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

#include "HardwareSerial.h"

#include <driver/uart.h>
#include <driver/uart_types.h>
#include <components/system.h>

namespace {
uart_id_t to_uart_id(int uart_nr) {
    if (uart_nr < 0) {
        return static_cast<uart_id_t>(bk_get_printf_port());
    }

    switch (uart_nr) {
        case 0:
            return UART_ID_0;
        case 1:
            return UART_ID_1;
        case 2:
            return UART_ID_2;
        case 3:
            return UART_ID_3;
        default:
            return static_cast<uart_id_t>(bk_get_printf_port());
    }
}
}

HardwareSerial Serial(-1);

HardwareSerial::HardwareSerial(int uart_nr)
    : m_uart_nr(uart_nr), m_started(false), m_peek(-1) {
}

void HardwareSerial::begin(unsigned long baud) {
    begin(baud, SERIAL_8N1);
}

void HardwareSerial::begin(unsigned long baud, uint16_t config) {
    (void)config;
    const uart_id_t id = to_uart_id(m_uart_nr);
    const int printf_port = bk_get_printf_port();

    bk_uart_driver_init();

    if (id == static_cast<uart_id_t>(printf_port)) {
        bk_uart_set_baud_rate(id, static_cast<uint32_t>(baud));
        m_started = true;
        m_peek = -1;
        return;
    }

    if (bk_uart_is_in_used(id)) {
        bk_uart_set_baud_rate(id, static_cast<uint32_t>(baud));
        m_started = true;
        m_peek = -1;
        return;
    }

    uart_config_t uart_config = {
        .baud_rate = static_cast<uint32_t>(baud),
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_FLOWCTRL_DISABLE,
        .src_clk = UART_SCLK_XTAL_26M,
        .rx_dma_en = UART_DMA_DISABLE,
        .tx_dma_en = UART_DMA_DISABLE,
    };

    bk_uart_init(id, &uart_config);
    m_started = true;
    m_peek = -1;
}

void HardwareSerial::end() {
    const uart_id_t id = to_uart_id(m_uart_nr);

    if (!m_started) {
        return;
    }

    if (id != static_cast<uart_id_t>(bk_get_printf_port())) {
        bk_uart_deinit(id);
    }

    m_started = false;
    m_peek = -1;
}

int HardwareSerial::available() {
    if (!m_started) {
        return 0;
    }
    if (m_peek >= 0) {
        return 1;
    }
    uint8_t value = 0;
    const int read_len = bk_uart_read_bytes(to_uart_id(m_uart_nr), &value, 1, 0);
    if (read_len == 1) {
        m_peek = value;
        return 1;
    }
    return 0;
}

int HardwareSerial::read() {
    if (!m_started) {
        return -1;
    }
    if (m_peek >= 0) {
        const int value = m_peek;
        m_peek = -1;
        return value;
    }
    uint8_t value = 0;
    const int read_len = bk_uart_read_bytes(to_uart_id(m_uart_nr), &value, 1, 0);
    return (read_len == 1) ? value : -1;
}

int HardwareSerial::peek() {
    return available() ? m_peek : -1;
}

void HardwareSerial::flush() {
    if (m_started) {
        bk_uart_wait_tx_over(to_uart_id(m_uart_nr));
    }
}

int HardwareSerial::availableForWrite() {
    return m_started ? 1 : 0;
}

size_t HardwareSerial::write(uint8_t value) {
    if (!m_started) {
        begin(115200);
    }
    return (bk_uart_write_bytes(to_uart_id(m_uart_nr), &value, 1) == BK_OK) ? 1 : 0;
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size) {
    if (!m_started) {
        begin(115200);
    }
    return (bk_uart_write_bytes(to_uart_id(m_uart_nr), buffer, size) == BK_OK) ? size : 0;
}