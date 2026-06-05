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

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>

extern "C" {
#include <common/bk_include.h>
#include <components/bluetooth/bk_ble.h>
#include <components/bluetooth/bk_ble_types.h>
#include <components/bluetooth/bk_dm_bluetooth.h>
}

#include "ArduinoBLE.h"
#include "utility/BLEGattRegistry.h"
#include "BLECharacteristic.cpp"
#include "BLEService.cpp"
#include "utility/BLEGattRegistry.cpp"
#include "BLEStringCharacteristic.cpp"
#include "BLETypedCharacteristics.cpp"

namespace {
constexpr uint8_t kUnknownActvIdx = 0xFFU;

volatile bool g_ble_cmd_done = false;
volatile ble_err_t g_ble_cmd_status = BK_ERR_BLE_FAIL;
volatile ble_cmd_t g_ble_last_cmd = BLE_CMD_NONE;

extern "C" void arduino_ble_cmd_cb(ble_cmd_t cmd, ble_cmd_param_t *param) {
    g_ble_last_cmd = cmd;
    g_ble_cmd_status = param ? param->status : BK_ERR_BLE_FAIL;
    g_ble_cmd_done = true;
}

void fill_adv_params(ble_adv_param_t &param) {
    std::memset(&param, 0, sizeof(param));
    param.own_addr_type = 0;
    param.adv_type = 0;
    param.chnl_map = 7;
    param.adv_prop = 3;
    param.adv_intv_min = 0x120;
    param.adv_intv_max = 0x160;
    param.prim_phy = 1;
    param.second_phy = 1;
}

bool appendServiceUuidField(const String &uuid, uint8_t *adv_data, size_t *adv_len, size_t adv_capacity) {
    if (uuid.length() == 0) {
        return true;
    }

    String hex;
    hex.reserve(uuid.length());
    for (const char *p = uuid.c_str(); *p != '\0'; ++p) {
        const char ch = *p;
        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            hex.concat(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }

    uint8_t uuid_bytes[16] = {0};
    size_t uuid_len = 0;
    if (hex.length() == 4) {
        uuid_len = 2;
    } else if (hex.length() == 32) {
        uuid_len = 16;
    } else {
        return false;
    }

    for (size_t i = 0; i < uuid_len; ++i) {
        const size_t src_index = hex.length() - ((i + 1) * 2);
        const char *src = hex.c_str() + src_index;
        uuid_bytes[i] = static_cast<uint8_t>(strtoul(String(src, 2).c_str(), nullptr, 16));
    }

    const uint8_t ad_type = (uuid_len == 2) ? 0x03 : 0x07;
    if ((*adv_len + uuid_len + 2) > adv_capacity) {
        return false;
    }
    adv_data[(*adv_len)++] = static_cast<uint8_t>(uuid_len + 1);
    adv_data[(*adv_len)++] = ad_type;
    std::memcpy(&adv_data[*adv_len], uuid_bytes, uuid_len);
    *adv_len += uuid_len;
    return true;
}
}

namespace {
class BekenBLELocalDevice : public BLELocalDevice {
public:
    BekenBLELocalDevice();

    int begin() override;
    void end() override;
    bool connected() const override;
    bool disconnect() override;
    bool setAdvertisedServiceUuid(const char *advertisedServiceUuid) override;
    bool setAdvertisedService(const BLEService &service) override;
    bool setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength) override;
    bool setLocalName(const char *localName) override;
    void setDeviceName(const char *deviceName) override;
    void addService(BLEService &service) override;
    int advertise() override;
    void stopAdvertise() override;
    void restartAdvertising(void);

private:
    bool ensureInitialized(void);
    bool ensureActivity(void);
    bool waitForCommand(uint8_t expected_cmd, uint32_t timeout_ms);
    bool setAdvertisingPayload(const uint8_t *payload, size_t len);
    bool setScanResponsePayload(const uint8_t *payload, size_t len);

    bool m_initialized;
    bool m_advertising;
    uint8_t m_actv_idx;
    String m_local_name;
    String m_device_name;
    String m_service_uuid;
    std::vector<uint8_t> m_manufacturer_data;
};
}  // namespace

static BekenBLELocalDevice g_ble_local_device;
BLELocalDevice &BLE = g_ble_local_device;

static void onBleDisconnected(void) {
    g_ble_local_device.restartAdvertising();
}

BekenBLELocalDevice::BekenBLELocalDevice()
    : m_initialized(false),
      m_advertising(false),
      m_actv_idx(kUnknownActvIdx) {
}

bool BekenBLELocalDevice::waitForCommand(uint8_t expected_cmd, uint32_t timeout_ms) {
    const uint32_t step_ms = 20;
    for (uint32_t waited = 0; waited < timeout_ms; waited += step_ms) {
        if (g_ble_cmd_done && g_ble_last_cmd == expected_cmd) {
            return g_ble_cmd_status == BK_ERR_BLE_SUCCESS;
        }
        delay(step_ms);
    }
    return false;
}

bool BekenBLELocalDevice::ensureInitialized(void) {
    if (m_initialized) {
        return true;
    }
    BLEGattRegistry::ensureNoticeCallback();
    BLEGattRegistry::setOnDisconnect(onBleDisconnected);
    if (bk_bluetooth_init() != BK_OK) {
        return false;
    }
    BLEGattRegistry::ensureEventCallback();
    delay(100);
    m_initialized = true;
    return true;
}

void BekenBLELocalDevice::restartAdvertising(void) {
    if (m_actv_idx == kUnknownActvIdx || !m_advertising) {
        return;
    }
    bk_ble_start_advertising(m_actv_idx, 0, nullptr);
}

bool BekenBLELocalDevice::ensureActivity(void) {
    if (m_actv_idx != kUnknownActvIdx) {
        return true;
    }
    if (!ensureInitialized()) {
        return false;
    }

    m_actv_idx = bk_ble_get_idle_actv_idx_handle();
    if (m_actv_idx == kUnknownActvIdx) {
        return false;
    }

    ble_adv_param_t adv_param;
    fill_adv_params(adv_param);
    g_ble_cmd_done = false;
    g_ble_last_cmd = BLE_CMD_NONE;
    g_ble_cmd_status = BK_ERR_BLE_FAIL;
    if (bk_ble_create_advertising(m_actv_idx, &adv_param, arduino_ble_cmd_cb) != BK_ERR_BLE_SUCCESS) {
        m_actv_idx = kUnknownActvIdx;
        return false;
    }

    if (!waitForCommand(BLE_CREATE_ADV, 2000)) {
        m_actv_idx = kUnknownActvIdx;
        return false;
    }

    return true;
}

int BekenBLELocalDevice::begin() {
    if (!ensureInitialized()) {
        return 0;
    }
    return 1;
}

void BekenBLELocalDevice::end() {
    stopAdvertise();
}

bool BekenBLELocalDevice::connected() const {
    return BLEGattRegistry::isConnected();
}

bool BekenBLELocalDevice::disconnect() {
    if (!BLEGattRegistry::isConnected()) {
        return true;
    }
    return bk_ble_disconnect(BLEGattRegistry::connectionHandle()) == BK_ERR_BLE_SUCCESS;
}

bool BekenBLELocalDevice::setAdvertisedServiceUuid(const char *advertisedServiceUuid) {
    m_service_uuid = advertisedServiceUuid ? advertisedServiceUuid : "";
    return true;
}

bool BekenBLELocalDevice::setAdvertisedService(const BLEService &service) {
    return setAdvertisedServiceUuid(service.uuid());
}

bool BekenBLELocalDevice::setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength) {
    if (!manufacturerData || manufacturerDataLength < 0) {
        return false;
    }
    m_manufacturer_data.assign(manufacturerData, manufacturerData + manufacturerDataLength);
    return true;
}

bool BekenBLELocalDevice::setLocalName(const char *localName) {
    m_local_name = localName ? localName : "";
    return true;
}

void BekenBLELocalDevice::setDeviceName(const char *deviceName) {
    m_device_name = deviceName ? deviceName : "";
}

void BekenBLELocalDevice::addService(BLEService &service) {
    BLELocalDevice::addService(service);
    if (m_service_uuid.length() == 0) {
        m_service_uuid = service.uuid();
    }
}

bool BekenBLELocalDevice::setAdvertisingPayload(const uint8_t *payload, size_t len) {
    if (len > BK_BLE_MAX_ADV_DATA_LEN) {
        return false;
    }
    g_ble_cmd_done = false;
    g_ble_last_cmd = BLE_CMD_NONE;
    g_ble_cmd_status = BK_ERR_BLE_FAIL;
    if (bk_ble_set_adv_data(m_actv_idx, const_cast<uint8_t *>(payload), static_cast<uint8_t>(len), arduino_ble_cmd_cb) != BK_ERR_BLE_SUCCESS) {
        return false;
    }
    return waitForCommand(BLE_SET_ADV_DATA, 2000);
}

bool BekenBLELocalDevice::setScanResponsePayload(const uint8_t *payload, size_t len) {
    if (len > BK_BLE_MAX_ADV_DATA_LEN) {
        return false;
    }
    g_ble_cmd_done = false;
    g_ble_last_cmd = BLE_CMD_NONE;
    g_ble_cmd_status = BK_ERR_BLE_FAIL;
    if (bk_ble_set_scan_rsp_data(m_actv_idx, const_cast<uint8_t *>(payload), static_cast<uint8_t>(len), arduino_ble_cmd_cb) != BK_ERR_BLE_SUCCESS) {
        return false;
    }
    return waitForCommand(BLE_SET_RSP_DATA, 2000);
}

int BekenBLELocalDevice::advertise() {
    if (!begin()) {
        return 0;
    }
    if (!BLEGattRegistry::finalizeDatabase(services())) {
        return 0;
    }
    if (!ensureActivity()) {
        return 0;
    }

    const String effective_device_name = (m_device_name.length() != 0) ? m_device_name
        : ((m_local_name.length() != 0) ? m_local_name : String("arduino-beken"));
    const uint8_t name_len = static_cast<uint8_t>(effective_device_name.length());
    if (bk_ble_appm_set_dev_name(name_len, reinterpret_cast<uint8_t *>(const_cast<char *>(effective_device_name.c_str()))) != name_len) {
        return 0;
    }

    uint8_t adv_data[BK_BLE_MAX_ADV_DATA_LEN] = {0};
    size_t adv_len = 0;
    adv_data[adv_len++] = 0x02;
    adv_data[adv_len++] = 0x01;
    adv_data[adv_len++] = 0x06;

    if (!appendServiceUuidField(m_service_uuid, adv_data, &adv_len, sizeof(adv_data))) {
        return 0;
    }

    if (!m_manufacturer_data.empty()) {
        if ((m_manufacturer_data.size() + 2 + adv_len) > sizeof(adv_data)) {
            return 0;
        }
        adv_data[adv_len++] = static_cast<uint8_t>(m_manufacturer_data.size() + 1);
        adv_data[adv_len++] = 0xFF;
        std::memcpy(&adv_data[adv_len], m_manufacturer_data.data(), m_manufacturer_data.size());
        adv_len += m_manufacturer_data.size();
    }

    if (!setAdvertisingPayload(adv_data, adv_len)) {
        return 0;
    }

    if (m_local_name.length() != 0) {
        uint8_t scan_rsp[BK_BLE_MAX_ADV_DATA_LEN] = {0};
        if ((m_local_name.length() + 2) <= sizeof(scan_rsp)) {
            scan_rsp[0] = static_cast<uint8_t>(m_local_name.length() + 1);
            scan_rsp[1] = 0x09;
            std::memcpy(&scan_rsp[2], m_local_name.c_str(), m_local_name.length());
            if (!setScanResponsePayload(scan_rsp, m_local_name.length() + 2)) {
                return 0;
            }
        }
    }

    g_ble_cmd_done = false;
    g_ble_last_cmd = BLE_CMD_NONE;
    g_ble_cmd_status = BK_ERR_BLE_FAIL;
    if (bk_ble_start_advertising(m_actv_idx, 0, arduino_ble_cmd_cb) != BK_ERR_BLE_SUCCESS) {
        return 0;
    }

    m_advertising = waitForCommand(BLE_START_ADV, 2000);
    return m_advertising ? 1 : 0;
}

void BekenBLELocalDevice::stopAdvertise() {
    if (m_actv_idx == kUnknownActvIdx || !m_advertising) {
        return;
    }

    g_ble_cmd_done = false;
    g_ble_last_cmd = BLE_CMD_NONE;
    g_ble_cmd_status = BK_ERR_BLE_FAIL;
    if (bk_ble_stop_advertising(m_actv_idx, arduino_ble_cmd_cb) != BK_ERR_BLE_SUCCESS) {
        return;
    }

    m_advertising = !waitForCommand(BLE_STOP_ADV, 2000);
}

