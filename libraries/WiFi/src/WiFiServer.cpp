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

#include "WiFiServer.h"

#include "WiFiClient.h"

WiFiServer::WiFiServer(uint16_t port) : _port(port) {
}

WiFiClient WiFiServer::available(uint8_t *) {
    return WiFiClient();
}

void WiFiServer::begin() {
}

size_t WiFiServer::write(uint8_t) {
    return 0;
}

size_t WiFiServer::write(const uint8_t *, size_t) {
    return 0;
}

uint8_t WiFiServer::status() {
    return 0;
}