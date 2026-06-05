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

#include "Ticker.h"

extern "C" {
#include <os/os.h>
}

Ticker::Ticker() : m_timer{}, m_callback(nullptr), m_repeat(false), m_active(false) {
}

Ticker::~Ticker() {
    detach();
}

void Ticker::timerThunk(void *arg) {
    Ticker *ticker = static_cast<Ticker *>(arg);
    if (!ticker || !ticker->m_callback) {
        return;
    }
    ticker->m_callback();
    if (!ticker->m_repeat) {
        ticker->detach();
    }
}

void Ticker::start(uint32_t milliseconds, ticker_callback_t callback, bool repeat) {
    detach();
    m_callback = callback;
    m_repeat = repeat;
    m_active = true;
    rtos_init_timer(&m_timer, milliseconds, timerThunk, this);
    rtos_start_timer(&m_timer);
}

void Ticker::attach(float seconds, ticker_callback_t callback) {
    attach_ms(static_cast<uint32_t>(seconds * 1000.0f), callback);
}

void Ticker::attach_ms(uint32_t milliseconds, ticker_callback_t callback) {
    start(milliseconds, callback, true);
}

void Ticker::once_ms(uint32_t milliseconds, ticker_callback_t callback) {
    start(milliseconds, callback, false);
}

void Ticker::detach() {
    if (!m_active) {
        m_active = false;
        return;
    }
    rtos_stop_timer(&m_timer);
    rtos_deinit_timer(&m_timer);
    m_callback = nullptr;
    m_active = false;
}

bool Ticker::active() const {
    return m_active;
}