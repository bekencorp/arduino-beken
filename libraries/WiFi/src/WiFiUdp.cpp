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

#include "WiFiUdp.h"

extern "C" {
#include "utility/wl_definitions.h"
}

WiFiUDP::WiFiUDP() : _sock(SOCK_NOT_AVAIL), _port(0) {
}

uint8_t WiFiUDP::begin(uint16_t port) {
    _port = port;
    return 0;
}

void WiFiUDP::stop() {
}

int WiFiUDP::beginPacket(IPAddress, uint16_t) {
    return 0;
}

int WiFiUDP::beginPacket(const char *, uint16_t) {
    return 0;
}

int WiFiUDP::endPacket() {
    return 0;
}

size_t WiFiUDP::write(uint8_t) {
    return 0;
}

size_t WiFiUDP::write(const uint8_t *, size_t) {
    return 0;
}

int WiFiUDP::parsePacket() {
    return 0;
}

int WiFiUDP::available() {
    return 0;
}

int WiFiUDP::read() {
    return -1;
}

int WiFiUDP::read(unsigned char *, size_t) {
    return 0;
}

int WiFiUDP::peek() {
    return -1;
}

void WiFiUDP::flush() {
}

IPAddress WiFiUDP::remoteIP() {
    return arduino::INADDR_NONE;
}

uint16_t WiFiUDP::remotePort() {
    return 0;
}