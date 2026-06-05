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

#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <os/os.h>
#ifdef __cplusplus
}
#endif

#include "api/HardwareI2C.h"

class TwoWire : public arduino::HardwareI2C {
public:
    explicit TwoWire(uint8_t bus_num = 0);

    void begin() override;
    void begin(uint8_t address) override;
    bool begin(uint8_t sda, uint8_t scl, uint32_t frequency = 100000);
    void end() override;
    void setClock(uint32_t freq) override;

    void beginTransmission(uint8_t address) override;
    uint8_t endTransmission(bool sendStop) override;
    uint8_t endTransmission(void) override;

    size_t write(uint8_t data) override;
    size_t write(const uint8_t *data, size_t size) override;

    size_t requestFrom(uint8_t address, size_t size, bool sendStop) override;
    size_t requestFrom(uint8_t address, size_t size) override;
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    void onReceive(void (*callback)(int)) override;
    void onRequest(void (*callback)(void)) override;

private:
    bool ensureInit();
    bool beginMaster(uint8_t sda, uint8_t scl, uint32_t frequency);
    bool beginSlave(uint8_t sda, uint8_t scl, uint32_t frequency);
    void slaveServiceTask();
    static void slaveServiceThreadEntry(beken_thread_arg_t arg);

    uint8_t m_bus_num;
    uint8_t m_address;
    bool m_initialized;
    bool m_slave_mode;
    bool m_slave_thread_running;
    beken_thread_t m_slave_thread;
    bool m_queue_init;
    beken_queue_t m_i2c_queue;
    bool m_next_slave_event_write;
    uint8_t m_tx_buffer[64];
    size_t m_tx_length;
    bool m_request_pending;
    uint8_t m_rx_buffer[64];
    size_t m_rx_length;
    size_t m_rx_index;
    uint32_t m_frequency;
    void (*m_on_receive)(int);
    void (*m_on_request)(void);
    bool initSlaveEventQueue();
};

extern TwoWire Wire;