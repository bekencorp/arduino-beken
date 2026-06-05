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

#include "HardwareTimer.h"

extern "C" {
#include <driver/timer.h>
}

namespace {
HardwareTimer *g_hardware_timers[TIMER_ID_MAX] = {};
bool g_timer_driver_initialized = false;

void hardwareTimerThunk(timer_id_t timer_id) {
    if (timer_id >= TIMER_ID_MAX) {
        return;
    }

    HardwareTimer *timer = g_hardware_timers[timer_id];
    if (!timer) {
        return;
    }

    timer->invoke();
}
}

HardwareTimer::HardwareTimer(uint8_t timer_id)
    : m_timer_id(timer_id),
      m_callback(nullptr),
      m_period_ms(0),
      m_period_us(0),
      m_initialized(false),
      m_running(false) {
}

HardwareTimer::~HardwareTimer() {
    stop();
}

bool HardwareTimer::begin() {
    if (m_timer_id >= TIMER_ID_MAX) {
        return false;
    }

    if (!g_timer_driver_initialized) {
        if (bk_timer_driver_init() != BK_OK) {
            return false;
        }
        g_timer_driver_initialized = true;
    }

    g_hardware_timers[m_timer_id] = this;
    m_initialized = true;
    return true;
}

void HardwareTimer::attachInterrupt(hardware_timer_callback_t callback) {
    m_callback = callback;
}

bool HardwareTimer::start(uint32_t period_ms) {
    if (!m_initialized && !begin()) {
        return false;
    }

    stop();
    if (bk_timer_start(static_cast<timer_id_t>(m_timer_id), period_ms, hardwareTimerThunk) != BK_OK) {
        return false;
    }

    m_period_ms = period_ms;
    m_period_us = static_cast<uint64_t>(period_ms) * 1000ULL;
    m_running = true;
    return true;
}

bool HardwareTimer::startMicroseconds(uint64_t period_us) {
    if (!m_initialized && !begin()) {
        return false;
    }

    stop();
    if (bk_timer_start_us(static_cast<timer_id_t>(m_timer_id), period_us, hardwareTimerThunk) != BK_OK) {
        return false;
    }

    m_period_us = period_us;
    m_period_ms = static_cast<uint32_t>(period_us / 1000ULL);
    m_running = true;
    return true;
}

void HardwareTimer::stop() {
    if (!m_running) {
        return;
    }

    bk_timer_stop(static_cast<timer_id_t>(m_timer_id));
    m_running = false;
}

uint32_t HardwareTimer::read() const {
    if (m_timer_id >= TIMER_ID_MAX) {
        return 0;
    }

    return bk_timer_get_cnt(static_cast<timer_id_t>(m_timer_id));
}

uint32_t HardwareTimer::period() const {
    if (m_timer_id >= TIMER_ID_MAX) {
        return 0;
    }

    return bk_timer_get_period(static_cast<timer_id_t>(m_timer_id));
}

bool HardwareTimer::running() const {
    if (m_timer_id >= TIMER_ID_MAX) {
        return false;
    }

    return (bk_timer_get_enable_status() & (1U << m_timer_id)) != 0;
}

uint8_t HardwareTimer::id() const {
    return m_timer_id;
}

void HardwareTimer::invoke() const {
    if (m_callback) {
        m_callback();
    }
}