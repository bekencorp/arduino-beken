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

#include "Arduino.h"

#include <driver/adc.h>
#include <driver/hal/hal_adc_types.h>

int analogRead(uint8_t pin) {
    // Use digitalPinToAnalogChannel to get the ADC channel for this pin
    int channel = digitalPinToAnalogChannel(pin);
    if (channel < 0) {
        return -1;
    }

    adc_chan_t chan = static_cast<adc_chan_t>(channel);
    uint16_t value = 0;

#if CONFIG_SARADC_V1P2
    adc_config_t config = {};
    config.chan = chan;
    config.adc_mode = ADC_CONTINUOUS_MODE;
    config.clk = 0x30e035;
    config.src_clk = ADC_SCLK_XTAL;
    config.saturate_mode = ADC_SATURATE_MODE_3;
    config.steady_ctrl = 7;
    config.sample_rate = 0;
    config.adc_filter = 0;

    if (bk_adc_acquire() != BK_OK) {
        return -1;
    }
    if (bk_adc_channel_init(&config) != BK_OK) {
        bk_adc_release();
        return -1;
    }
    if (bk_adc_channel_read(chan, &value, 1000) != BK_OK) {
        bk_adc_channel_deinit(chan);
        bk_adc_release();
        return -1;
    }
    bk_adc_channel_deinit(chan);
    bk_adc_release();
#else
    if (bk_adc_init(chan) != BK_OK) {
        return -1;
    }
    if (bk_adc_start() != BK_OK) {
        bk_adc_deinit(chan);
        return -1;
    }
    if (bk_adc_read(&value, 1000) != BK_OK) {
        bk_adc_stop();
        bk_adc_deinit(chan);
        return -1;
    }
    bk_adc_stop();
    bk_adc_deinit(chan);
#endif

    return static_cast<int>(value);
}