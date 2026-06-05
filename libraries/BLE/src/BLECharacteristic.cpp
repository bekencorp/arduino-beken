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

#include "BLECharacteristic.h"

#include <cstring>

#include "utility/BLEGattRegistry.h"

BLECharacteristic::BLECharacteristic(const char *uuid,
                                     unsigned int properties,
                                     int valueSize,
                                     bool fixedLength)
    : m_uuid(uuid ? uuid : ""),
      m_properties(properties),
      m_value_size(valueSize > 0 ? static_cast<size_t>(valueSize) : 0),
      m_fixed_length(fixedLength),
      m_value() {
    if (m_value_size > 0) {
        m_value.reserve(m_value_size);
    }
}

int BLECharacteristic::writeValue(const uint8_t *value, int length) {
    if (!value || length < 0) {
        return 0;
    }
    storeValueBytes(value, static_cast<size_t>(length));
    BLEGattRegistry::notifyCharacteristic(*this);
    return 1;
}

int BLECharacteristic::writeValue(uint8_t value) {
    return writeValue(&value, 1);
}

int BLECharacteristic::writeValue(const char *value) {
    return value ? writeValue(reinterpret_cast<const uint8_t *>(value), std::strlen(value)) : 0;
}

int BLECharacteristic::writeValue(const String &value) {
    return writeValue(reinterpret_cast<const uint8_t *>(value.c_str()), value.length());
}