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

#include <driver/gpio.h>
#include <driver/uart.h>
extern "C" {
#include <os/os.h>
}
extern "C" void __libc_init_array(void);

namespace {
constexpr uint32_t kArduinoLoopPriority = 4;
constexpr uint32_t kArduinoLoopStackSize = 8192;

beken_thread_t g_loop_thread = nullptr;
bool g_cxx_runtime_initialized = false;

void ensure_cxx_runtime_initialized() {
    if (g_cxx_runtime_initialized) {
        return;
    }

    // Run sketch/library global constructors only after BK runtime init has
    // completed, but still before Arduino setup()/loop() starts.
    __libc_init_array();
    g_cxx_runtime_initialized = true;
}

void arduino_loop_task(beken_thread_arg_t) {
    setup();
    while (true) {
        loop();
        yield();
    }
}
}

void setup(void);
void loop(void);

extern "C" void user_app_main(void) {
    ensure_cxx_runtime_initialized();
    rtos_create_thread(
        &g_loop_thread,
        kArduinoLoopPriority,
        "arduino",
        arduino_loop_task,
        kArduinoLoopStackSize,
        nullptr
    );
}

void arduino_delay(unsigned long ms) {
    rtos_delay_milliseconds(ms);
}

void delayMicroseconds(unsigned int us) {
    if (us >= 1000) {
        rtos_delay_milliseconds(us / 1000);
    } else {
        volatile unsigned int count = us * 16;
        while (count--) {
            __asm__ volatile("nop");
        }
    }
}

unsigned long millis(void) {
    return rtos_get_time();
}

unsigned long micros(void) {
    return rtos_get_time() * 1000UL;
}

void yield(void) {
    rtos_delay_milliseconds(1);
}