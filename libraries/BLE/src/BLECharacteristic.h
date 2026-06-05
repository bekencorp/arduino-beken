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

#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "Arduino.h"
#include "BLEProperty.h"

class BLEDescriptor;
class BLEGattRegistry;

class BLECharacteristic {
public:
    BLECharacteristic(const char *uuid, unsigned int properties, int valueSize, bool fixedLength = false);

    const char *uuid() const { return m_uuid.c_str(); }
    const String &uuidString() const { return m_uuid; }
    unsigned int properties() const { return m_properties; }
    int valueSize() const { return static_cast<int>(m_value_size > 0 ? m_value_size : m_value.size()); }
    int valueLength() const { return static_cast<int>(m_content_len); }

    int writeValue(const uint8_t *value, int length);
    int writeValue(uint8_t value);
    int writeValue(const char *value);
    int writeValue(const String &value);

    int readValue(uint8_t *buffer, int length) const {
        if (!buffer || length <= 0) {
            return 0;
        }
        const size_t to_copy = std::min(static_cast<size_t>(length), m_value.size());
        for (size_t i = 0; i < to_copy; ++i) {
            buffer[i] = m_value[i];
        }
        return static_cast<int>(to_copy);
    }

    const unsigned char *value() const {
        return m_value.empty() ? nullptr : m_value.data();
    }

    void addDescriptor(BLEDescriptor &descriptor) { m_descriptors.push_back(&descriptor); }
    size_t descriptorCount() const { return m_descriptors.size(); }

    void setNotifyEnabled(bool enabled) { m_notify_enabled = enabled; }
    void writeFromPeer(const uint8_t *data, size_t len) { applyPeerWrite(data, len); }
    uint8_t *gattReadDataPtr() { return m_value.empty() ? nullptr : m_value.data(); }
    uint8_t *gattAttBufferPtr() { return m_att_buffer.empty() ? nullptr : m_att_buffer.data(); }
    uint16_t gattReadDataLen() const {
        if (m_content_len > 0) {
            return static_cast<uint16_t>(m_content_len);
        }
        if (m_value_size > 0) {
            return static_cast<uint16_t>(m_value_size);
        }
        return static_cast<uint16_t>(m_value.size());
    }
    uint8_t *cccdValuePtr() { return m_cccd_value; }

private:
    friend class BLEGattRegistry;

    void applyPeerWrite(const uint8_t *data, size_t len) {
        storeValueBytes(data, len);
    }

    void storeValueBytes(const uint8_t *data, size_t len) {
        if (!data || len == 0) {
            if (m_value_size > 0) {
                m_value.assign(m_value_size, 0);
                m_att_buffer.assign(m_value_size, 0);
                m_content_len = 0;
            } else {
                m_value.clear();
                m_content_len = 0;
            }
            return;
        }

        const size_t requested = len;
        if (m_value_size > 0 && requested > m_value_size) {
            return;
        }

        if (m_value_size > 0) {
            if (m_value.size() != m_value_size) {
                m_value.assign(m_value_size, 0);
            } else {
                std::fill(m_value.begin(), m_value.end(), 0);
            }
            if (m_att_buffer.size() != m_value_size) {
                m_att_buffer.assign(m_value_size, 0);
            } else {
                std::fill(m_att_buffer.begin(), m_att_buffer.end(), 0);
            }
            std::memcpy(m_value.data(), data, requested);
            std::memcpy(m_att_buffer.data(), data, requested);
            m_content_len = requested;
            return;
        }

        m_value.assign(data, data + requested);
        m_content_len = requested;
    }

    void ensureGattValueBuffer() {
        if (m_value_size > 0) {
            m_value.assign(m_value_size, 0);
            m_att_buffer.assign(m_value_size, 0);
            m_content_len = 0;
            return;
        }
        if (m_value.empty()) {
            m_value.push_back(0);
            m_content_len = 1;
        }
    }

    uint8_t *gattValuePtr() { return m_value.data(); }

    uint16_t gattValueLen() const {
        return gattReadDataLen();
    }

    String m_uuid;
    unsigned int m_properties;
    size_t m_value_size;
    size_t m_content_len = 0;
    bool m_fixed_length;
    std::vector<uint8_t> m_value;
    std::vector<uint8_t> m_att_buffer;
    std::vector<BLEDescriptor *> m_descriptors;
    uint16_t m_gatt_service_handle = 0;
    uint16_t m_gatt_char_handle = 0;
    bool m_gatt_registered = false;
    bool m_notify_enabled = false;
    uint8_t m_cccd_value[2] = {0, 0};
};

#endif