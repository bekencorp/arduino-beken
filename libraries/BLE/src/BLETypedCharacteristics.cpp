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

#include "BLETypedCharacteristics.h"

BLEBoolCharacteristic::BLEBoolCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<bool>(uuid, permissions) {
}

BLEBooleanCharacteristic::BLEBooleanCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<bool>(uuid, permissions) {
}

BLECharCharacteristic::BLECharCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<char>(uuid, permissions) {
}

BLEUnsignedCharCharacteristic::BLEUnsignedCharCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<unsigned char>(uuid, permissions) {
}

BLEByteCharacteristic::BLEByteCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<byte>(uuid, permissions) {
}

BLEShortCharacteristic::BLEShortCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<short>(uuid, permissions) {
}

BLEUnsignedShortCharacteristic::BLEUnsignedShortCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<unsigned short>(uuid, permissions) {
}

BLEWordCharacteristic::BLEWordCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<word>(uuid, permissions) {
}

BLEIntCharacteristic::BLEIntCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<int>(uuid, permissions) {
}

BLEUnsignedIntCharacteristic::BLEUnsignedIntCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<unsigned int>(uuid, permissions) {
}

BLELongCharacteristic::BLELongCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<long>(uuid, permissions) {
}

BLEUnsignedLongCharacteristic::BLEUnsignedLongCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<unsigned long>(uuid, permissions) {
}

BLEFloatCharacteristic::BLEFloatCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<float>(uuid, permissions) {
}

BLEDoubleCharacteristic::BLEDoubleCharacteristic(const char* uuid, unsigned int permissions) :
  BLETypedCharacteristic<double>(uuid, permissions) {
}