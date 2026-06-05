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

#include "SPI.h"

#include <common/bk_include.h>
#include <driver/spi.h>
#include <driver/spi_types.h>
#include <driver/hal/hal_spi_types.h>

namespace {
spi_id_t toSpiId(uint8_t bus_num) {
    switch (bus_num) {
        case 0:
            return SPI_ID_0;
#if (SOC_SPI_UNIT_NUM > 1)
        case 1:
            return SPI_ID_1;
#endif
#if (SOC_SPI_UNIT_NUM > 2)
        case 2:
            return SPI_ID_2;
#endif
        default:
            return SPI_ID_0;
    }
}

spi_mode_t toSpiMode(arduino::SPIMode mode) {
    switch (mode) {
        case SPI_MODE1:
            return SPI_POL_MODE_1;
        case SPI_MODE2:
            return SPI_POL_MODE_2;
        case SPI_MODE3:
            return SPI_POL_MODE_3;
        case SPI_MODE0:
        default:
            return SPI_POL_MODE_0;
    }
}

void applySpiMode(spi_config_t &config, arduino::SPIMode mode) {
    switch (mode) {
        case SPI_MODE1:
            config.polarity = SPI_POLARITY_LOW;
            config.phase = SPI_PHASE_2ND_EDGE;
            break;
        case SPI_MODE2:
            config.polarity = SPI_POLARITY_HIGH;
            config.phase = SPI_PHASE_1ST_EDGE;
            break;
        case SPI_MODE3:
            config.polarity = SPI_POLARITY_HIGH;
            config.phase = SPI_PHASE_2ND_EDGE;
            break;
        case SPI_MODE0:
        default:
            config.polarity = SPI_POLARITY_LOW;
            config.phase = SPI_PHASE_1ST_EDGE;
            break;
    }
}

spi_bit_order_t toBitOrder(BitOrder order) {
    return (order == LSBFIRST) ? SPI_LSB_FIRST : SPI_MSB_FIRST;
}

spi_role_t toSpiRole(arduino::SPIBusMode mode) {
    return (mode == arduino::SPI_PERIPHERAL) ? SPI_ROLE_SLAVE : SPI_ROLE_MASTER;
}

bool setBitWidth(spi_id_t id, spi_bit_width_t bit_width) {
    return bk_spi_set_bit_width(id, bit_width) == BK_OK;
}

bool duplexTransfer(spi_id_t id, const void *tx_data, void *rx_data, size_t size) {
#if CONFIG_SPI_DMA
    if (bk_spi_dma_duplex_init(id) != BK_OK) {
        return false;
    }

    const bk_err_t result = bk_spi_dma_duplex_xfer(id, tx_data, size, rx_data, size);
    const bk_err_t deinit_result = bk_spi_dma_duplex_deinit(id);
    return (result == BK_OK) && (deinit_result == BK_OK);
#else
    return bk_spi_transmit(id, tx_data, size, rx_data, size) == BK_OK;
#endif
}
}

SPIClass SPI(0);

SPIClass::SPIClass(uint8_t bus_num) : m_bus_num(bus_num), m_initialized(false) {
}

bool SPIClass::ensureInit() {
    if (m_initialized) {
        return true;
    }
    begin();
    return m_initialized;
}

void SPIClass::begin() {
    if (m_initialized) {
        return;
    }
    if (bk_spi_driver_init() != BK_OK) {
        return;
    }
    spi_config_t config = {};
    config.baud_rate = arduino::DEFAULT_SPI_SETTINGS.getClockFreq();
    config.role = SPI_ROLE_MASTER;
    config.bit_width = SPI_BIT_WIDTH_8BITS;
    config.wire_mode = SPI_4WIRE_MODE;
    config.bit_order = SPI_MSB_FIRST;
    applySpiMode(config, SPI_MODE0);
    m_initialized = (bk_spi_init(toSpiId(m_bus_num), &config) == BK_OK);
}

void SPIClass::end() {
    if (!m_initialized) {
        return;
    }
    bk_spi_deinit(toSpiId(m_bus_num));
    m_initialized = false;
}

void SPIClass::applySettings(const SPISettings &settings) {
    const spi_id_t id = toSpiId(m_bus_num);
    bk_spi_set_role(id, toSpiRole(settings.getBusMode()));
    bk_spi_set_baud_rate(id, settings.getClockFreq());
    bk_spi_set_mode(id, toSpiMode(settings.getDataMode()));
    bk_spi_set_bit_order(id, toBitOrder(settings.getBitOrder()));
    bk_spi_set_bit_width(id, SPI_BIT_WIDTH_8BITS);
}

void SPIClass::usingInterrupt(int interruptNumber) {
    (void)interruptNumber;
}

void SPIClass::notUsingInterrupt(int interruptNumber) {
    (void)interruptNumber;
}

void SPIClass::beginTransaction(SPISettings settings) {
    if (!ensureInit()) {
        return;
    }
    applySettings(settings);
}

void SPIClass::endTransaction() {
}

void SPIClass::attachInterrupt() {
}

void SPIClass::detachInterrupt() {
}

uint8_t SPIClass::transfer(uint8_t data) {
    uint8_t rx = 0xff;
    if (!ensureInit()) {
        return rx;
    }
    duplexTransfer(toSpiId(m_bus_num), &data, &rx, sizeof(data));
    return rx;
}

uint16_t SPIClass::transfer16(uint16_t data) {
    uint16_t rx = 0xffff;
    if (!ensureInit()) {
        return rx;
    }

    const spi_id_t id = toSpiId(m_bus_num);
    if (!setBitWidth(id, SPI_BIT_WIDTH_16BITS)) {
        return rx;
    }

    if (!duplexTransfer(id, &data, &rx, sizeof(data))) {
        setBitWidth(id, SPI_BIT_WIDTH_8BITS);
        return 0xffff;
    }

    setBitWidth(id, SPI_BIT_WIDTH_8BITS);
    return rx;
}

void SPIClass::transfer(void *data, size_t size) {
    if (!ensureInit() || !data || size == 0) {
        return;
    }
    duplexTransfer(toSpiId(m_bus_num), data, data, size);
}