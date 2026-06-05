// Copyright 2025-2026 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "BLEGattRegistry.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <common/bk_include.h>
#include <components/bluetooth/bk_ble.h>
#include <components/bluetooth/bk_ble_types.h>
#include <components/bluetooth/bk_dm_ble.h>
#include <components/bluetooth/bk_dm_ble_types.h>
#include <components/bluetooth/bk_dm_bluetooth.h>
}

#include "../BLECharacteristic.h"
#include "../BLEProperty.h"
#include "../BLEService.h"
#include "Arduino.h"

extern "C" uint32_t arduino_ble_gatt_event_cb(ble_event_enum_t event, void *param);
extern "C" bk_err_t arduino_ble_gatt_db_handler(uint8_t conn_handle, GATT_DB_HANDLE *handle, GATT_DB_PARAMS *params);
extern "C" void arduino_ble_legacy_notice_cb(ble_notice_t notice, void *param);

namespace {
constexpr uint8_t kInvalidConnHandle = 0xFFU;
constexpr uint16_t kGattAttrHandlesPerService = 30U;
constexpr uint16_t kLegacyPrfTaskBase = 11U;

static const uint8_t kDeclCharacteristicUuid[16] = {0x03, 0x28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const uint8_t kDescClientCharCfgUuid[16] = {0x02, 0x29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

volatile bool g_legacy_db_create_done = false;
volatile uint8_t g_legacy_db_create_status = 0xFFU;

struct LegacyCharBinding {
    BLECharacteristic *characteristic;
    uint16_t prf_id;
    uint16_t value_att_idx;
    uint16_t desc_att_idx;
};

std::vector<ble_attm_desc_t> g_legacy_att_db;
std::vector<LegacyCharBinding> g_legacy_bindings;

String normalizeUuidHex(const char *uuid) {
    String hex;
    if (!uuid) {
        return hex;
    }
    hex.reserve(std::strlen(uuid));
    for (const char *p = uuid; *p != '\0'; ++p) {
        const char ch = *p;
        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            hex.concat(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }
    return hex;
}

bool parseUuid16(const char *uuid, uint16_t &out) {
    const String hex = normalizeUuidHex(uuid);
    if (hex.length() != 4) {
        return false;
    }
    out = static_cast<uint16_t>(strtoul(hex.c_str(), nullptr, 16));
    return true;
}

bool fillGattUuid(const char *uuid, GATT_DB_UUID_TYPE &out) {
    uint16_t uuid16 = 0;
    if (!parseUuid16(uuid, uuid16)) {
        return false;
    }
    out.uuid_format = ATT_16_BIT_UUID_FORMAT;
    out.uuid.uuid_16 = uuid16;
    return true;
}

uint16_t mapGattProperties(unsigned int properties) {
    uint16_t mapped = 0;
    if (properties & BLERead) {
        mapped |= GATT_DB_CHAR_READ_PROPERTY;
    }
    if (properties & BLEWriteWithoutResponse) {
        mapped |= GATT_DB_CHAR_WRITE_WITHOUT_RSP_PROPERTY;
    }
    if (properties & BLEWrite) {
        mapped |= GATT_DB_CHAR_WRITE_PROPERTY;
    }
    if (properties & BLENotify) {
        mapped |= GATT_DB_CHAR_NOTIFY_PROPERTY;
    }
    if (properties & BLEIndicate) {
        mapped |= GATT_DB_CHAR_INDICATE_PROPERTY;
    }
    if (properties & BLEBroadcast) {
        mapped |= GATT_DB_CHAR_BROADCAST_PROPERTY;
    }
    return mapped;
}

uint16_t mapGattPermissions(unsigned int properties) {
    uint16_t perm = GATT_DB_PERM_NONE;
    if (properties & BLERead) {
        perm |= GATT_DB_PERM_READ;
    }
    if (properties & (BLEWrite | BLEWriteWithoutResponse)) {
        perm |= GATT_DB_PERM_WRITE;
    }
    if (perm == GATT_DB_PERM_NONE) {
        perm = GATT_DB_PERM_READ;
    }
    return perm;
}

uint16_t mapLegacyCharPerm(unsigned int properties) {
    uint16_t perm = 0;
    if (properties & BLERead) {
        perm |= BK_BLE_PERM_SET(RD, ENABLE);
    }
    if (properties & BLENotify) {
        perm |= BK_BLE_PERM_SET(NTF, ENABLE);
    }
    if (properties & BLEIndicate) {
        perm |= BK_BLE_PERM_SET(IND, ENABLE);
    }
    if (properties & BLEWrite) {
        perm |= BK_BLE_PERM_SET(WRITE_REQ, ENABLE);
    }
    if (properties & BLEWriteWithoutResponse) {
        perm |= BK_BLE_PERM_SET(WRITE_COMMAND, ENABLE);
    }
    if (perm == 0) {
        perm = BK_BLE_PERM_SET(RD, ENABLE);
    }
    return perm;
}

void resetLegacyDbWaitState() {
    g_legacy_db_create_done = false;
    g_legacy_db_create_status = 0xFFU;
}

LegacyCharBinding *findLegacyBindingByCharacteristic(BLECharacteristic *characteristic) {
    for (LegacyCharBinding &binding : g_legacy_bindings) {
        if (binding.characteristic == characteristic) {
            return &binding;
        }
    }
    return nullptr;
}

LegacyCharBinding *findLegacyBindingByWrite(uint16_t prf_id, uint16_t att_idx) {
    for (LegacyCharBinding &binding : g_legacy_bindings) {
        if (binding.prf_id != prf_id) {
            continue;
        }
        if (binding.value_att_idx == att_idx || binding.desc_att_idx == att_idx) {
            return &binding;
        }
    }
    return nullptr;
}
}  // namespace

std::vector<BLECharacteristic *> BLEGattRegistry::s_characteristics;
bool BLEGattRegistry::s_gatt_db_finalized = false;
bool BLEGattRegistry::s_use_legacy_stack = true;
bool BLEGattRegistry::s_event_callback_set = false;
bool BLEGattRegistry::s_notice_callback_set = false;
uint8_t BLEGattRegistry::s_conn_handle = kInvalidConnHandle;
BLEGattRegistry::DisconnectHandler BLEGattRegistry::s_on_disconnect = nullptr;

bool BLEGattRegistry::isConnected() {
    return s_conn_handle != kInvalidConnHandle;
}

uint8_t BLEGattRegistry::connectionHandle() {
    return s_conn_handle;
}

void BLEGattRegistry::setOnDisconnect(DisconnectHandler handler) {
    s_on_disconnect = handler;
}

void BLEGattRegistry::onDisconnected() {
    if (s_on_disconnect) {
        s_on_disconnect();
    }
}

bool BLEGattRegistry::usesEthermindStack() {
    return bk_ble_get_host_stack_type() == BK_BLE_HOST_STACK_TYPE_ETHERMIND;
}

void BLEGattRegistry::ensureEventCallback() {
    if (s_event_callback_set) {
        return;
    }
    bk_ble_set_event_callback(arduino_ble_gatt_event_cb);
    s_event_callback_set = true;
}

void BLEGattRegistry::ensureNoticeCallback() {
    if (s_notice_callback_set) {
        return;
    }
    bk_ble_set_notice_cb(arduino_ble_legacy_notice_cb);
    s_notice_callback_set = true;
}

bool BLEGattRegistry::waitForLegacyDatabaseCreated(uint32_t timeout_ms) {
    const uint32_t step_ms = 20;
    for (uint32_t waited = 0; waited < timeout_ms; waited += step_ms) {
        if (g_legacy_db_create_done) {
            return g_legacy_db_create_status == 0U;
        }
        delay(step_ms);
    }
    return false;
}

BLECharacteristic *BLEGattRegistry::findCharacteristic(uint16_t service_handle, uint16_t char_handle) {
    for (BLECharacteristic *characteristic : s_characteristics) {
        if (!characteristic || !characteristic->m_gatt_registered) {
            continue;
        }
        if (characteristic->m_gatt_service_handle == service_handle &&
            characteristic->m_gatt_char_handle == char_handle) {
            return characteristic;
        }
    }
    return nullptr;
}

bool BLEGattRegistry::registerCharacteristicInGatt(BLEService &service, BLECharacteristic &characteristic) {
    if (characteristic.m_gatt_registered) {
        return true;
    }
    if (!service.m_gatt_registered) {
        return false;
    }

    GATT_DB_UUID_TYPE char_uuid;
    if (!fillGattUuid(characteristic.uuid(), char_uuid)) {
        return false;
    }

    characteristic.ensureGattValueBuffer();

    ATT_VALUE char_value;
    char_value.val = characteristic.gattValuePtr();
    char_value.len = characteristic.gattValueLen();
    char_value.actual_len = char_value.len;

    const uint16_t perm = mapGattPermissions(characteristic.properties());
    const uint16_t property = mapGattProperties(characteristic.properties());
    uint16_t char_handle = 0;

    if (bk_ble_gatt_db_add_characteristic(service.m_gatt_service_handle,
                                          &char_uuid,
                                          perm,
                                          property,
                                          &char_value,
                                          &char_handle) != BK_ERR_BLE_SUCCESS) {
        return false;
    }

    characteristic.m_gatt_service_handle = service.m_gatt_service_handle;
    characteristic.m_gatt_char_handle = char_handle;
    characteristic.m_gatt_registered = true;
    if (findCharacteristic(service.m_gatt_service_handle, char_handle) == nullptr) {
        s_characteristics.push_back(&characteristic);
    }

    if (characteristic.properties() & (BLENotify | BLEIndicate)) {
        GATT_DB_UUID_TYPE desc_uuid;
        ATT_VALUE desc_value;

        desc_uuid.uuid_format = ATT_16_BIT_UUID_FORMAT;
        desc_uuid.uuid.uuid_16 = GATT_CLIENT_CONFIG;
        desc_value.val = characteristic.cccdValuePtr();
        desc_value.len = 2U;
        desc_value.actual_len = desc_value.len;

        if (bk_ble_gatt_db_add_characteristic_descriptor(service.m_gatt_service_handle,
                                                         char_handle,
                                                         &desc_uuid,
                                                         GATT_DB_PERM_READ | GATT_DB_PERM_WRITE,
                                                         &desc_value) != BK_ERR_BLE_SUCCESS) {
            return false;
        }
    }

    return true;
}

bool BLEGattRegistry::registerService(BLEService &service) {
    if (service.m_gatt_registered) {
        for (BLECharacteristic *characteristic : service.characteristics()) {
            if (characteristic && !characteristic->m_gatt_registered) {
                if (!registerCharacteristicInGatt(service, *characteristic)) {
                    return false;
                }
            }
        }
        return true;
    }

    GATT_DB_SERVICE_INFO service_info;
    std::memset(&service_info, 0, sizeof(service_info));
    if (!fillGattUuid(service.uuid(), service_info.uuid)) {
        return false;
    }

    service_info.is_primary = 1;
    service_info.link_req = GATT_DB_SER_SUPPORT_ANY_LINK_TYPE;
    service_info.sec_req = GATT_DB_SER_NO_SECURITY_PROPERTY;

    uint16_t service_handle = 0;
    if (bk_ble_gatt_db_add_service(&service_info,
                                   kGattAttrHandlesPerService,
                                   &service_handle) != BK_ERR_BLE_SUCCESS) {
        return false;
    }

    service.m_gatt_service_handle = service_handle;
    service.m_gatt_registered = true;

    for (BLECharacteristic *characteristic : service.characteristics()) {
        if (!characteristic) {
            continue;
        }
        if (!registerCharacteristicInGatt(service, *characteristic)) {
            return false;
        }
    }

    return true;
}

bool BLEGattRegistry::finalizeLegacyDatabase(const std::vector<BLEService *> &services) {
    ensureNoticeCallback();
    g_legacy_att_db.clear();
    g_legacy_bindings.clear();

    uint16_t prf_id = kLegacyPrfTaskBase;
    for (BLEService *service : services) {
        if (!service) {
            continue;
        }

        uint16_t service_uuid = 0;
        if (!parseUuid16(service->uuid(), service_uuid)) {
            return false;
        }

        g_legacy_att_db.clear();
        g_legacy_att_db.reserve(4U * service->characteristics().size() + 1U);

        ble_attm_desc_t service_decl;
        std::memset(&service_decl, 0, sizeof(service_decl));
        service_decl.uuid[0] = static_cast<uint8_t>(service_uuid & 0xFFU);
        service_decl.uuid[1] = static_cast<uint8_t>(service_uuid >> 8);
        service_decl.perm = BK_BLE_PERM_SET(RD, ENABLE);
        g_legacy_att_db.push_back(service_decl);

        for (BLECharacteristic *characteristic : service->characteristics()) {
            if (!characteristic) {
                continue;
            }

            uint16_t char_uuid = 0;
            if (!parseUuid16(characteristic->uuid(), char_uuid)) {
                return false;
            }

            characteristic->ensureGattValueBuffer();

            ble_attm_desc_t char_decl;
            std::memset(&char_decl, 0, sizeof(char_decl));
            std::memcpy(char_decl.uuid, kDeclCharacteristicUuid, sizeof(kDeclCharacteristicUuid));
            char_decl.perm = BK_BLE_PERM_SET(RD, ENABLE);
            g_legacy_att_db.push_back(char_decl);

            const uint16_t value_att_idx = static_cast<uint16_t>(g_legacy_att_db.size());
            ble_attm_desc_t char_value;
            std::memset(&char_value, 0, sizeof(char_value));
            char_value.uuid[0] = static_cast<uint8_t>(char_uuid & 0xFFU);
            char_value.uuid[1] = static_cast<uint8_t>(char_uuid >> 8);
            char_value.perm = mapLegacyCharPerm(characteristic->properties());
            char_value.ext_perm = BK_BLE_PERM_SET(RI, ENABLE) | BK_BLE_PERM_SET(UUID_LEN, UUID_16);
            char_value.max_size = characteristic->gattReadDataLen();
            if (char_value.max_size == 0) {
                char_value.max_size = 1;
            }
            char_value.p_value_context = characteristic->gattAttBufferPtr();
            g_legacy_att_db.push_back(char_value);

            LegacyCharBinding binding;
            binding.characteristic = characteristic;
            binding.prf_id = prf_id;
            binding.value_att_idx = value_att_idx;
            binding.desc_att_idx = 0xFFFFU;

            if (characteristic->properties() & (BLENotify | BLEIndicate)) {
                binding.desc_att_idx = static_cast<uint16_t>(g_legacy_att_db.size());
                ble_attm_desc_t cccd;
                std::memset(&cccd, 0, sizeof(cccd));
                std::memcpy(cccd.uuid, kDescClientCharCfgUuid, sizeof(kDescClientCharCfgUuid));
                cccd.perm = BK_BLE_PERM_SET(RD, ENABLE) | BK_BLE_PERM_SET(WRITE_REQ, ENABLE);
                g_legacy_att_db.push_back(cccd);
            }

            g_legacy_bindings.push_back(binding);
            characteristic->m_gatt_registered = true;
            characteristic->m_gatt_service_handle = prf_id;
            characteristic->m_gatt_char_handle = value_att_idx;
            if (findCharacteristic(prf_id, value_att_idx) == nullptr) {
                s_characteristics.push_back(characteristic);
            }
        }

        if (g_legacy_att_db.empty()) {
            return false;
        }

        bk_ble_db_cfg db_cfg;
        std::memset(&db_cfg, 0, sizeof(db_cfg));
        db_cfg.att_db = g_legacy_att_db.data();
        db_cfg.att_db_nb = static_cast<uint8_t>(g_legacy_att_db.size());
        db_cfg.prf_task_id = prf_id;
        db_cfg.start_hdl = 0;
        db_cfg.svc_perm = BK_BLE_PERM_SET(SVC_UUID_LEN, UUID_16);
        db_cfg.uuid[0] = static_cast<uint8_t>(service_uuid & 0xFFU);
        db_cfg.uuid[1] = static_cast<uint8_t>(service_uuid >> 8);

        resetLegacyDbWaitState();
        if (bk_ble_create_db(&db_cfg) != BK_ERR_BLE_SUCCESS) {
            return false;
        }
        if (!waitForLegacyDatabaseCreated(500)) {
            return false;
        }
        delay(50);

        service->m_gatt_registered = true;
        service->m_gatt_service_handle = prf_id;
        ++prf_id;
    }

    return !g_legacy_bindings.empty();
}

bool BLEGattRegistry::finalizeEthermindDatabase(const std::vector<BLEService *> &services) {
    ensureEventCallback();

    for (BLEService *service : services) {
        if (!service) {
            continue;
        }
        if (!registerService(*service)) {
            return false;
        }
    }

    if (bk_ble_gatt_db_add_completed() != BK_ERR_BLE_SUCCESS) {
        return false;
    }
    delay(100);
    if (bk_ble_gatt_db_set_callback(arduino_ble_gatt_db_handler) != BK_ERR_BLE_SUCCESS) {
        return false;
    }

    return true;
}

extern "C" uint32_t arduino_ble_gatt_event_cb(ble_event_enum_t event, void *param) {
    switch (event) {
        case BK_DM_BLE_EVENT_CONNECT: {
            const ble_conn_att_t *conn = static_cast<const ble_conn_att_t *>(param);
            if (conn) {
                BLEGattRegistry::setConnectionHandle(conn->conn_handle);
            }
            break;
        }
        case BK_DM_BLE_EVENT_DISCONNECT:
            BLEGattRegistry::setConnectionHandle(kInvalidConnHandle);
            BLEGattRegistry::onDisconnected();
            break;
        default:
            break;
    }
    return 0;
}

extern "C" void arduino_ble_legacy_notice_cb(ble_notice_t notice, void *param) {
    switch (notice) {
        case BLE_5_STACK_OK:
            break;
        case BLE_5_CONNECT_EVENT: {
            const ble_conn_ind_t *conn = static_cast<const ble_conn_ind_t *>(param);
            if (conn) {
                BLEGattRegistry::setConnectionHandle(conn->conn_idx);
            }
            break;
        }
        case BLE_5_DISCONNECT_EVENT: {
            BLEGattRegistry::setConnectionHandle(kInvalidConnHandle);
            BLEGattRegistry::onDisconnected();
            break;
        }
        case BLE_5_ATT_INFO_REQ: {
            ble_att_info_req_t *att_req = static_cast<ble_att_info_req_t *>(param);
            if (att_req) {
                att_req->length = 128;
                att_req->status = BK_ERR_BLE_SUCCESS;
            }
            break;
        }
        case BLE_5_READ_EVENT: {
            const ble_read_req_t *read_req = static_cast<const ble_read_req_t *>(param);
            if (!read_req) {
                break;
            }
            LegacyCharBinding *binding = findLegacyBindingByWrite(read_req->prf_id, read_req->att_idx);
            if (!binding || !binding->characteristic) {
                break;
            }
            if (read_req->att_idx == binding->value_att_idx ||
                read_req->att_idx == binding->desc_att_idx) {
                BLECharacteristic *characteristic = binding->characteristic;
                const uint16_t length = characteristic->gattReadDataLen();
                uint8_t *value = characteristic->gattReadDataPtr();
                uint16_t respond_len = length;
                if (read_req->att_idx == binding->desc_att_idx) {
                    value = characteristic->cccdValuePtr();
                    respond_len = 2U;
                }
                if (value && respond_len > 0) {
                    bk_ble_read_response_value(read_req->conn_idx,
                                               respond_len,
                                               value,
                                               read_req->prf_id,
                                               read_req->att_idx);
                }
            }
            break;
        }
        case BLE_5_CREATE_DB: {
            const ble_create_db_t *created = static_cast<const ble_create_db_t *>(param);
            g_legacy_db_create_status = created ? created->status : 0U;
            g_legacy_db_create_done = true;
            break;
        }
        case BLE_5_WRITE_EVENT: {
            const ble_write_req_t *write_req = static_cast<const ble_write_req_t *>(param);
            if (!write_req) {
                break;
            }
            LegacyCharBinding *binding = findLegacyBindingByWrite(write_req->prf_id, write_req->att_idx);
            if (!binding || !binding->characteristic) {
                break;
            }
            if (write_req->att_idx == binding->desc_att_idx) {
                const uint16_t config =
                    static_cast<uint16_t>((static_cast<uint16_t>(write_req->value[1]) << 8) | write_req->value[0]);
                binding->characteristic->setNotifyEnabled(config == 0x0001U || config == 0x0002U);
            } else if (write_req->att_idx == binding->value_att_idx) {
                binding->characteristic->writeFromPeer(write_req->value, write_req->len);
            }
            break;
        }
        default:
            break;
    }
}

extern "C" bk_err_t arduino_ble_gatt_db_handler(uint8_t conn_handle, GATT_DB_HANDLE *handle, GATT_DB_PARAMS *params) {
    if (!handle || !params) {
        return 0;
    }

    BLECharacteristic *characteristic =
        BLEGattRegistry::findCharacteristic(handle->service_id, handle->char_id);
    if (!characteristic) {
        return 0;
    }

    switch (params->db_op) {
        case GATT_DB_CHAR_PEER_READ_REQ:
            bk_ble_gatt_read_resp(conn_handle,
                                  characteristic->gattReadDataPtr(),
                                  characteristic->gattReadDataLen());
            break;
        case GATT_DB_CHAR_PEER_CLI_CNFG_WRITE_REQ: {
            const uint16_t config =
                static_cast<uint16_t>((static_cast<uint16_t>(params->value.val[1]) << 8) |
                                      params->value.val[0]);
            characteristic->setNotifyEnabled(config == GATT_CLI_CNFG_NOTIFICATION ||
                                               config == GATT_CLI_CNFG_INDICATION);
            break;
        }
        case GATT_DB_CHAR_PEER_WRITE_REQ:
        case GATT_DB_CHAR_PEER_WRITE_CMD:
            characteristic->writeFromPeer(params->value.val, params->value.len);
            break;
        default:
            break;
    }

    return 0;
}

void BLEGattRegistry::onCharacteristicAdded(BLEService &service, BLECharacteristic &characteristic) {
    (void)service;
    (void)characteristic;
}

bool BLEGattRegistry::finalizeDatabase(const std::vector<BLEService *> &services) {
    if (s_gatt_db_finalized) {
        return true;
    }
    if (services.empty()) {
        return false;
    }

    ensureNoticeCallback();
    ensureEventCallback();
    delay(50);

    // Prefer legacy bk_ble_create_db: dm_ble GATT APIs may be stubbed when BLE_5_X is off.
    if (!usesEthermindStack()) {
        s_use_legacy_stack = true;
        if (finalizeLegacyDatabase(services)) {
            s_gatt_db_finalized = true;
            return true;
        }
        return false;
    }

    // Ethermind dm_ble path (bk_ble_gatt_db_add_service); fall back to legacy on failure.
    s_use_legacy_stack = false;
    if (finalizeEthermindDatabase(services)) {
        s_gatt_db_finalized = true;
        return true;
    }

    s_use_legacy_stack = true;
    if (finalizeLegacyDatabase(services)) {
        s_gatt_db_finalized = true;
        return true;
    }

    return false;
}

bool BLEGattRegistry::notifyCharacteristic(BLECharacteristic &characteristic) {
    if (!characteristic.m_gatt_registered || !characteristic.m_notify_enabled) {
        return false;
    }
    if (!(characteristic.properties() & (BLENotify | BLEIndicate))) {
        return false;
    }
    if (s_conn_handle == kInvalidConnHandle) {
        return false;
    }

    const uint16_t length = characteristic.gattReadDataLen();
    uint8_t *value = characteristic.gattReadDataPtr();
    if (!value || length == 0) {
        return false;
    }

    if (s_use_legacy_stack) {
        const LegacyCharBinding *binding = findLegacyBindingByCharacteristic(&characteristic);
        if (!binding) {
            return false;
        }
        return bk_ble_send_noti_value(s_conn_handle,
                                      length,
                                      value,
                                      binding->prf_id,
                                      binding->value_att_idx) == BK_ERR_BLE_SUCCESS;
    }

    return bk_ble_send_notify(s_conn_handle,
                              characteristic.m_gatt_service_handle,
                              characteristic.m_gatt_char_handle,
                              value,
                              length) == BK_ERR_BLE_SUCCESS;
}