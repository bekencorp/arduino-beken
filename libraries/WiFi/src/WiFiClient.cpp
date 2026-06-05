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

#include "WiFiClient.h"

uint16_t WiFiClient::_srcport = 0;

WiFiClient::WiFiClient() : _sock(SOCK_NOT_AVAIL), _socket(0) {
}

WiFiClient::WiFiClient(uint8_t sock) : _sock(sock), _socket(0) {
}

uint8_t WiFiClient::status() {
    return WL_DISCONNECTED;
}

int WiFiClient::connect(IPAddress, uint16_t) {
    return 0;
}

int WiFiClient::connect(const char *, uint16_t) {
    return 0;
}

size_t WiFiClient::write(uint8_t) {
    return 0;
}

size_t WiFiClient::write(const uint8_t *, size_t) {
    return 0;
}

int WiFiClient::available() {
    return 0;
}

int WiFiClient::read() {
    return -1;
}

int WiFiClient::read(uint8_t *, size_t) {
    return 0;
}

int WiFiClient::peek() {
    return -1;
}

void WiFiClient::flush() {
}

void WiFiClient::stop() {
}

uint8_t WiFiClient::connected() {
    return 0;
}

WiFiClient::operator bool() {
    return false;
}

uint8_t WiFiClient::getFirstSocket() {
    return SOCK_NOT_AVAIL;
}