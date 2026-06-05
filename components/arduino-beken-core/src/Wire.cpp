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

#include "Wire.h"

#if defined(CONFIG_I2C) && CONFIG_I2C
#include <common/bk_include.h>
#include <driver/i2c.h>
#include <driver/i2c_types.h>
#include <driver/hal/hal_i2c_types.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <os/os.h>
#ifdef __cplusplus
}
#endif
#endif

#include "pins_arduino.h"

#if defined(CONFIG_I2C) && CONFIG_I2C
namespace {
i2c_id_t toBusId(uint8_t bus_num) {
    switch (bus_num) {
        case 0:
            return I2C_ID_0;
#if (SOC_I2C_UNIT_NUM > 1)
        case 1:
            return I2C_ID_1;
#endif
#if (SOC_I2C_UNIT_NUM > 2)
        case 2:
            return I2C_ID_2;
#endif
        default:
            return I2C_ID_0;
    }
}
}
#endif

TwoWire Wire(0);

TwoWire::TwoWire(uint8_t bus_num)
    : m_bus_num(bus_num),
      m_address(0),
      m_initialized(false),
      m_slave_mode(false),
      m_slave_thread_running(false),
      m_slave_thread(nullptr),
      m_queue_init(false),
      m_next_slave_event_write(false),
      m_tx_length(0),
      m_request_pending(false),
      m_rx_length(0),
      m_rx_index(0),
      m_frequency(100000),
      m_on_receive(nullptr),
      m_on_request(nullptr) {
}

bool TwoWire::ensureInit() {
    if (m_initialized) {
        return true;
    }
    begin();
    return m_initialized;
}

void TwoWire::begin() {
    begin(PIN_WIRE_SDA, PIN_WIRE_SCL, m_frequency);
}

void TwoWire::begin(uint8_t address) {
    m_address = address;
    m_slave_mode = (address != 0);
    if (m_initialized) {
        end();
    }
    begin(PIN_WIRE_SDA, PIN_WIRE_SCL, m_frequency);
}

bool TwoWire::begin(uint8_t sda, uint8_t scl, uint32_t frequency) {
    m_frequency = frequency;
    if (m_slave_mode) {
        return beginSlave(sda, scl, frequency);
    }
    return beginMaster(sda, scl, frequency);
}

bool TwoWire::beginMaster(uint8_t sda, uint8_t scl, uint32_t frequency) {
    m_frequency = frequency;
    (void)sda;
    (void)scl;
#if defined(CONFIG_I2C) && CONFIG_I2C
    bk_i2c_driver_init();
    i2c_config_t config = {

        .baud_rate = frequency,
        .addr_mode = I2C_ADDR_MODE_7BIT,
        .slave_addr = 0,
    };
    m_initialized = (bk_i2c_init(toBusId(m_bus_num), &config) == BK_OK);
    return m_initialized;
#else
    (void)frequency;
    return false;
#endif
}

bool TwoWire::beginSlave(uint8_t sda, uint8_t scl, uint32_t frequency) {
    m_frequency = frequency;
    (void)sda;
    (void)scl;
#if defined(CONFIG_I2C) && CONFIG_I2C
    bk_i2c_driver_init();
    i2c_config_t config = {
        .baud_rate = frequency,
        .addr_mode = I2C_ADDR_MODE_7BIT,
        .slave_addr = m_address,
    };
    m_initialized = (bk_i2c_init(toBusId(m_bus_num), &config) == BK_OK);
    if (m_initialized && (m_on_receive || m_on_request)) {
        if (!initSlaveEventQueue()) {
            bk_i2c_deinit(toBusId(m_bus_num));
            m_initialized = false;
            return false;
        }
        if (m_slave_thread == nullptr) {
            m_slave_thread_running = true;
            rtos_create_thread(&m_slave_thread, 4, "wire_slave", TwoWire::slaveServiceThreadEntry, 2048, this);
        }
    }
    return m_initialized;
#else
    (void)frequency;
    return false;
#endif
}

void TwoWire::end() {
    if (!m_initialized) {
        return;
    }
#if defined(CONFIG_I2C) && CONFIG_I2C
    if (m_slave_thread_running) {
        m_slave_thread_running = false;
        if (m_slave_thread) {
            rtos_thread_join(&m_slave_thread);
            m_slave_thread = nullptr;
        }
    }
    if (m_queue_init) {
        bk_i2c_queue_callback(toBusId(m_bus_num), NULL, NULL);
        rtos_deinit_queue(&m_i2c_queue);
        m_queue_init = false;
    }
    m_request_pending = false;
    bk_i2c_deinit(toBusId(m_bus_num));
#endif
    m_initialized = false;
}

void TwoWire::setClock(uint32_t freq) {
    m_frequency = freq;
    if (m_initialized) {
        end();
        begin();
    }
}

void TwoWire::beginTransmission(uint8_t address) {
    m_address = address;
    m_tx_length = 0;
}

uint8_t TwoWire::endTransmission(bool) {
    if (!ensureInit()) {
        return 4;
    }
    if (m_tx_length == 0) {
        return 0;
    }
#if defined(CONFIG_I2C) && CONFIG_I2C
    const bk_err_t ret = bk_i2c_master_write(toBusId(m_bus_num), m_address, m_tx_buffer, m_tx_length, 1000);
    m_tx_length = 0;

    switch(ret) {
        case BK_OK:
            return 0;
        case BK_ERR_I2C_ACK_TIMEOUT:
            return 2;
        case BK_ERR_I2C_BUS_BUSY_TIMEOUT:
            return 5;
        case BK_ERR_I2C_SCL_TIMEOUT:
            return 5;
        default:
            return 4;  // Other error
    }
#else
    m_tx_length = 0;
    return 4;
#endif
}

uint8_t TwoWire::endTransmission(void) {
    return endTransmission(true);
}

size_t TwoWire::write(uint8_t data) {
    if (m_tx_length >= sizeof(m_tx_buffer)) {
        return 0;
    }
    m_tx_buffer[m_tx_length++] = data;
    return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t size) {
    size_t written = 0;
    for (size_t i = 0; i < size; ++i) {
        written += write(data[i]);
    }
    return written;
}

size_t TwoWire::requestFrom(uint8_t address, size_t size, bool) {
    if (!ensureInit()) {
        return 0;
    }
    if (size > sizeof(m_rx_buffer)) {
        size = sizeof(m_rx_buffer);
    }
    m_rx_index = 0;
    m_rx_length = 0;
#if defined(CONFIG_I2C) && CONFIG_I2C
    const bk_err_t ret = bk_i2c_master_read(toBusId(m_bus_num), address, m_rx_buffer, size, 1000);
    if (ret == BK_OK) {
        m_rx_length = size;
    }
#else
    (void)address;
#endif
    return m_rx_length;
}

size_t TwoWire::requestFrom(uint8_t address, size_t size) {
    return requestFrom(address, size, true);
}

int TwoWire::available() {
    return static_cast<int>(m_rx_length - m_rx_index);
}

int TwoWire::read() {
    if (m_rx_index >= m_rx_length) {
        return -1;
    }
    return m_rx_buffer[m_rx_index++];
}

int TwoWire::peek() {
    if (m_rx_index >= m_rx_length) {
        return -1;
    }
    return m_rx_buffer[m_rx_index];
}

void TwoWire::flush() {
}

void TwoWire::onReceive(void (*callback)(int)) {
    m_on_receive = callback;
#if defined(CONFIG_I2C) && CONFIG_I2C
    if (m_initialized && m_slave_mode && (m_on_receive || m_on_request)) {
        if (!initSlaveEventQueue()) {
            return;
        }
        if (m_slave_thread == nullptr) {
            m_slave_thread_running = true;
            rtos_create_thread(&m_slave_thread, 4, "wire_slave", TwoWire::slaveServiceThreadEntry, 2048, this);
        }
    }
#endif
}

void TwoWire::onRequest(void (*callback)(void)) {
    m_on_request = callback;
#if defined(CONFIG_I2C) && CONFIG_I2C
    if (m_initialized && m_slave_mode && (m_on_receive || m_on_request)) {
        if (!initSlaveEventQueue()) {
            return;
        }
        if (m_slave_thread == nullptr) {
            m_slave_thread_running = true;
            rtos_create_thread(&m_slave_thread, 4, "wire_slave", TwoWire::slaveServiceThreadEntry, 2048, this);
        }
    }
#endif
}

#if defined(CONFIG_I2C) && CONFIG_I2C
bool TwoWire::initSlaveEventQueue() {
    if (m_queue_init) {
        return true;
    }

    if (rtos_init_queue(&m_i2c_queue, "wire_i2c", sizeof(i2c_cb_param_t), 10) != kNoErr) {
        return false;
    }
    bk_i2c_queue_callback(toBusId(m_bus_num), &m_i2c_queue, this);
    m_queue_init = true;
    return true;
}

void TwoWire::slaveServiceTask() {
    while (m_slave_thread_running) {
        if (!m_initialized || !m_slave_mode) {
            rtos_delay_milliseconds(50);
            continue;
        }

        bool eventHandled = false;
        bool handleRequestFirst = m_next_slave_event_write;

        if (handleRequestFirst) {
            if (m_on_request) {
                if (!m_request_pending) {
                    m_tx_length = 0;
                    m_on_request();
                    if (m_tx_length > 0) {
                        m_request_pending = true;
                    }
                }

                if (m_request_pending) {
                    const bk_err_t ret = bk_i2c_slave_write(toBusId(m_bus_num), m_tx_buffer, m_tx_length, 100);
                    if (ret == BK_OK) {
                        i2c_cb_param_t msg;
                        if (rtos_pop_from_queue(&m_i2c_queue, &msg, 100) == kNoErr && msg.work_mode == I2C_SLAVE_WRITE) {
                            eventHandled = true;
                        }
                    }
                    m_tx_length = 0;
                    m_request_pending = false;
                }
            }

            if (!eventHandled && m_on_receive) {
                m_rx_length = 0;
                const bk_err_t ret = bk_i2c_slave_read(toBusId(m_bus_num), m_rx_buffer, sizeof(m_rx_buffer), 100);
                if (ret == BK_OK) {
                    i2c_cb_param_t msg;
                    if (rtos_pop_from_queue(&m_i2c_queue, &msg, 100) == kNoErr && msg.work_mode == I2C_SLAVE_READ && msg.data_len <= sizeof(m_rx_buffer)) {
                        m_rx_length = static_cast<size_t>(msg.data_len);
                    } else {
                        m_rx_length = 0;
                    }
                    m_rx_index = 0;
                    if (m_rx_length > 0 && m_on_receive) {
                        m_on_receive(static_cast<int>(m_rx_length));
                    }
                    eventHandled = true;
                }
            }
        } else {
            if (m_on_receive) {
                m_rx_length = 0;
                const bk_err_t ret = bk_i2c_slave_read(toBusId(m_bus_num), m_rx_buffer, sizeof(m_rx_buffer), 100);
                if (ret == BK_OK) {
                    i2c_cb_param_t msg;
                    if (rtos_pop_from_queue(&m_i2c_queue, &msg, 100) == kNoErr && msg.work_mode == I2C_SLAVE_READ && msg.data_len <= sizeof(m_rx_buffer)) {
                        m_rx_length = static_cast<size_t>(msg.data_len);
                    } else {
                        m_rx_length = 0;
                    }
                    m_rx_index = 0;
                    if (m_rx_length > 0 && m_on_receive) {
                        m_on_receive(static_cast<int>(m_rx_length));
                    }
                    eventHandled = true;
                }
            }

            if (!eventHandled && m_on_request) {
                if (!m_request_pending) {
                    m_tx_length = 0;
                    m_on_request();
                    if (m_tx_length > 0) {
                        m_request_pending = true;
                    }
                }

                if (m_request_pending) {
                    const bk_err_t ret = bk_i2c_slave_write(toBusId(m_bus_num), m_tx_buffer, m_tx_length, 100);
                    if (ret == BK_OK) {
                        i2c_cb_param_t msg;
                        if (rtos_pop_from_queue(&m_i2c_queue, &msg, 100) == kNoErr && msg.work_mode == I2C_SLAVE_WRITE) {
                            eventHandled = true;
                        }
                    }
                    m_tx_length = 0;
                    m_request_pending = false;
                }
            }
        }

        if (eventHandled) {
            m_next_slave_event_write = !m_next_slave_event_write;
        } else {
            rtos_delay_milliseconds(50);
        }
    }
}
#endif

void TwoWire::slaveServiceThreadEntry(beken_thread_arg_t arg) {
    TwoWire *wire = reinterpret_cast<TwoWire *>(arg);
    if (wire) {
        wire->slaveServiceTask();
    }
    rtos_delete_thread(NULL);
}