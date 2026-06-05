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
#include <stddef.h>
#include <stdint.h>

#include "api/ArduinoAPI.h"
#include "HardwareSerial.h"
#include "HardwareTimer.h"
#include "pins_arduino.h"

void arduino_delay(unsigned long ms);
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
unsigned long millis(void);
unsigned long micros(void);
void yield(void);

void analogWrite(uint8_t pin, int value);
void analogWriteFrequency(uint8_t pin, uint32_t frequency);
void analogWriteResolution(uint8_t pin, uint8_t resolution_bits);

#ifdef __cplusplus
void setup(void);
void loop(void);
#endif

#define delay(ms) arduino_delay(ms)