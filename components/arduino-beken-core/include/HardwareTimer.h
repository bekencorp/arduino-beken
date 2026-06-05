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

#include <stdbool.h>
#include <stdint.h>

typedef void (*hardware_timer_callback_t)(void);

class HardwareTimer {
public:
    explicit HardwareTimer(uint8_t timer_id = 4);
    ~HardwareTimer();

    bool begin();
    void attachInterrupt(hardware_timer_callback_t callback);
    bool start(uint32_t period_ms);
    bool startMicroseconds(uint64_t period_us);
    void stop();

    uint32_t read() const;
    uint32_t period() const;
    bool running() const;
    uint8_t id() const;
    void invoke() const;

private:
    uint8_t m_timer_id;
    hardware_timer_callback_t m_callback;
    uint32_t m_period_ms;
    uint64_t m_period_us;
    bool m_initialized;
    bool m_running;

};