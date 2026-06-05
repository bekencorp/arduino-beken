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

#pragma once

#include <cstdint>
#include <vector>

class BLECharacteristic;
class BLEService;

class BLEGattRegistry {
public:
    static void ensureEventCallback();
    static void ensureNoticeCallback();
    static void onCharacteristicAdded(BLEService &service, BLECharacteristic &characteristic);
    static bool finalizeDatabase(const std::vector<BLEService *> &services);
    static bool notifyCharacteristic(BLECharacteristic &characteristic);
    static BLECharacteristic *findCharacteristic(uint16_t service_handle, uint16_t char_handle);
    static void setConnectionHandle(uint8_t conn_handle) { s_conn_handle = conn_handle; }
    static bool isConnected();
    static uint8_t connectionHandle();
    using DisconnectHandler = void (*)(void);
    static void setOnDisconnect(DisconnectHandler handler);
    static void onDisconnected();

private:
    static bool usesEthermindStack();
    static bool finalizeEthermindDatabase(const std::vector<BLEService *> &services);
    static bool finalizeLegacyDatabase(const std::vector<BLEService *> &services);
    static bool registerService(BLEService &service);
    static bool registerCharacteristicInGatt(BLEService &service, BLECharacteristic &characteristic);
    static bool waitForLegacyDatabaseCreated(uint32_t timeout_ms);

    static std::vector<BLECharacteristic *> s_characteristics;
    static bool s_gatt_db_finalized;
    static bool s_use_legacy_stack;
    static bool s_event_callback_set;
    static bool s_notice_callback_set;
    static uint8_t s_conn_handle;
    static DisconnectHandler s_on_disconnect;
};