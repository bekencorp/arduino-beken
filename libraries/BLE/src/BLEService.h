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

#include <vector>

#include "Arduino.h"

class BLECharacteristic;

class BLEService {
public:
    explicit BLEService(const char *uuid) : m_uuid(uuid ? uuid : "") {}

    const char *uuid() const { return m_uuid.c_str(); }
    const String &uuidString() const { return m_uuid; }
    void addCharacteristic(BLECharacteristic &characteristic);
    const std::vector<BLECharacteristic *> &characteristics() const { return m_characteristics; }

private:
    friend class BLEGattRegistry;

    String m_uuid;
    std::vector<BLECharacteristic *> m_characteristics;
    uint16_t m_gatt_service_handle = 0;
    bool m_gatt_registered = false;
};