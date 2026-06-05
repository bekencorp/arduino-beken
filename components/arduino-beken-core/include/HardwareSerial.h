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

#include <stdint.h>

#include "api/HardwareSerial.h"

class HardwareSerial : public arduino::HardwareSerial {
public:
    explicit HardwareSerial(int uart_nr);

    void begin(unsigned long baud) override;
    void begin(unsigned long baud, uint16_t config) override;
    void end() override;

    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    int availableForWrite() override;

    size_t write(uint8_t value) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    operator bool() override { return m_started; }

private:
    int m_uart_nr;
    bool m_started;
    int m_peek;
};

extern HardwareSerial Serial;