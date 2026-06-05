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

#include <vector>

#include "Arduino.h"

#include "../BLECharacteristic.h"
#include "../BLEService.h"

class BLELocalDevice {
public:
    BLELocalDevice() = default;
    virtual ~BLELocalDevice() = default;

    virtual int begin() = 0;
    virtual void end() = 0;
    virtual void poll() {}
    virtual void poll(unsigned long timeout) { delay(timeout); }
    virtual bool connected() const = 0;
    virtual bool disconnect() = 0;
    virtual bool setAdvertisedServiceUuid(const char *advertisedServiceUuid) = 0;
    virtual bool setAdvertisedService(const BLEService &service) = 0;
    virtual bool setManufacturerData(const uint8_t manufacturerData[], int manufacturerDataLength) = 0;
    virtual bool setLocalName(const char *localName) = 0;
    virtual void setDeviceName(const char *deviceName) = 0;
    virtual void setConnectionInterval(float, float) {}
    virtual void setSupervisionTimeout(uint16_t) {}
    virtual void addService(BLEService &service) { m_services.push_back(&service); }
    virtual int advertise() = 0;
    virtual void stopAdvertise() = 0;

protected:
    const std::vector<BLEService *> &services() const { return m_services; }

private:
    std::vector<BLEService *> m_services;
};

extern BLELocalDevice &BLE;
