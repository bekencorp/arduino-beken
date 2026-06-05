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

#include "BLEStringCharacteristic.h"

BLEStringCharacteristic::BLEStringCharacteristic(const char* uuid, unsigned int properties, int valueSize) :
  BLECharacteristic(uuid, properties, valueSize, false) {
}

int BLEStringCharacteristic::writeValue(const String& value) {
  return BLECharacteristic::writeValue(reinterpret_cast<const uint8_t*>(value.c_str()), value.length());
}

String BLEStringCharacteristic::value(void) {
  const unsigned char* raw = BLECharacteristic::value();
  const int len = valueLength();
  if (!raw || len <= 0) {
    return String();
  }
  return String(reinterpret_cast<const char*>(raw), len);
}