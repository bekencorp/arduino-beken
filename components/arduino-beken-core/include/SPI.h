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

#include "api/HardwareSPI.h"

using arduino::SPIBusMode;
using arduino::SPISettings;
using arduino::SPIMode;
using arduino::SPI_MODE0;
using arduino::SPI_MODE1;
using arduino::SPI_MODE2;
using arduino::SPI_MODE3;

class SPIClass : public arduino::HardwareSPI {
public:
    explicit SPIClass(uint8_t bus_num = 0);

    void begin() override;
    void end() override;
    void usingInterrupt(int interruptNumber) override;
    void notUsingInterrupt(int interruptNumber) override;
    void beginTransaction(SPISettings settings) override;
    void endTransaction() override;
    void attachInterrupt() override;
    void detachInterrupt() override;

    uint8_t transfer(uint8_t data) override;
    uint16_t transfer16(uint16_t data) override;
    void transfer(void *data, size_t size) override;

private:
    bool ensureInit();
    void applySettings(const SPISettings &settings);

    uint8_t m_bus_num;
    bool m_initialized;
};

extern SPIClass SPI;