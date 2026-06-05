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

#include <math.h>

extern "C" {
#include <driver/pwm.h>
}

namespace {
constexpr uint32_t kPwmSourceClockHz = 26000000U;

template <typename T>
T clampValue(T value, T low, T high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

struct PwmChannelState {
    uint8_t pin;
    uint32_t period_cycle;
    uint32_t frequency;
    uint8_t resolution_bits;
    bool initialized;
    bool started;
};

bool g_pwm_driver_initialized = false;
PwmChannelState g_pwm_channels[PWM_ID_MAX] = {};

uint32_t pwmMaxDuty(uint8_t resolution_bits) {
    const uint8_t clamped_bits = clampValue<uint8_t>(resolution_bits, 1, 15);
    return (1UL << clamped_bits) - 1UL;
}

bool ensurePwmDriverInitialized() {
    if (g_pwm_driver_initialized) {
        return true;
    }

    if (bk_pwm_driver_init() != BK_OK) {
        return false;
    }

    g_pwm_driver_initialized = true;
    return true;
}

bool pwmComputeConfig(uint8_t resolution_bits, uint32_t frequency, uint32_t *period_cycle, uint32_t *psc) {
    if (!period_cycle || !psc || frequency == 0) {
        return false;
    }

    const uint32_t max_duty = pwmMaxDuty(resolution_bits);
    const uint32_t effective_period = max_duty == 0 ? 1 : max_duty;
    const double divider = ceil(static_cast<double>(kPwmSourceClockHz) /
                                (static_cast<double>(frequency) * static_cast<double>(effective_period)));
    const uint32_t divider_u32 = clampValue<uint32_t>(static_cast<uint32_t>(divider), 1U, 256U);

    *period_cycle = effective_period;
    *psc = divider_u32 - 1U;
    return true;
}

}

void analogWriteFrequency(uint8_t pin, uint32_t frequency) {
    const int channel = digitalPinToPwmChannel(pin);
    if (channel < 0 || channel >= PWM_ID_MAX || frequency == 0) {
        return;
    }

    PwmChannelState &state = g_pwm_channels[channel];
    state.pin = pin;
    state.frequency = frequency;
    if (state.resolution_bits == 0) {
        state.resolution_bits = 8;
    }
}

void analogWriteResolution(uint8_t pin, uint8_t resolution_bits) {
    const int channel = digitalPinToPwmChannel(pin);
    if (channel < 0 || channel >= PWM_ID_MAX) {
        return;
    }

    PwmChannelState &state = g_pwm_channels[channel];
    state.pin = pin;
    state.resolution_bits = clampValue<uint8_t>(resolution_bits, 1, 15);
    if (state.frequency == 0) {
        state.frequency = 1000;
    }
}

void analogWrite(uint8_t pin, int value) {
    const int channel = digitalPinToPwmChannel(pin);
    if (channel < 0 || channel >= PWM_ID_MAX) {
        return;
    }

    if (!ensurePwmDriverInitialized()) {
        return;
    }

    PwmChannelState &state = g_pwm_channels[channel];
    state.pin = pin;
    if (state.frequency == 0) {
        state.frequency = 1000;
    }
    if (state.resolution_bits == 0) {
        state.resolution_bits = 8;
    }

    uint32_t period_cycle = 0;
    uint32_t psc = 0;
    if (!pwmComputeConfig(state.resolution_bits, state.frequency, &period_cycle, &psc)) {
        return;
    }

    const uint32_t max_duty = pwmMaxDuty(state.resolution_bits);
    const uint32_t duty_cycle = static_cast<uint32_t>(constrain(value, 0, static_cast<int>(max_duty)));

    const bool reconfigure = !state.initialized || state.period_cycle != period_cycle;
    if (reconfigure) {
        if (state.started) {
            bk_pwm_stop(static_cast<pwm_chan_t>(channel));
            state.started = false;
        }
        if (state.initialized) {
            bk_pwm_deinit(static_cast<pwm_chan_t>(channel));
            state.initialized = false;
        }

        pwm_init_config_t config = {};
        config.period_cycle = period_cycle;
        config.duty_cycle = duty_cycle;
        config.psc = psc;
        if (bk_pwm_init(static_cast<pwm_chan_t>(channel), &config) != BK_OK) {
            return;
        }

        state.period_cycle = period_cycle;
        state.initialized = true;
    }

    pwm_period_duty_config_t config = {};
    config.period_cycle = period_cycle;
    config.duty_cycle = duty_cycle;
    config.psc = psc;
    if (bk_pwm_set_period_duty(static_cast<pwm_chan_t>(channel), &config) != BK_OK) {
        return;
    }

    if (!state.started) {
        if (bk_pwm_start(static_cast<pwm_chan_t>(channel)) != BK_OK) {
            return;
        }
        state.started = true;
    }
}