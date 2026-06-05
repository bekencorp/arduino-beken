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

#include "WiFi.h"

#include <cstdio>
#include <cstring>

extern "C" {
#include <common/bk_include.h>
#include <components/event.h>
#include <modules/wifi.h>
#include <modules/wifi_types.h>
#include <os/mem.h>
#include <os/str.h>
}

namespace {
volatile bool g_scan_done = false;
volatile bool g_connect_done = false;
volatile bool g_connected = false;
bool g_scan_cb_registered = false;

extern "C" bk_err_t arduino_wifi_event_cb(void *, event_module_t, int event_id, void *) {
    if (event_id == EVENT_WIFI_SCAN_DONE) {
        g_scan_done = true;
    } else if (event_id == EVENT_WIFI_STA_CONNECTED) {
        g_connect_done = true;
        g_connected = true;
    } else if (event_id == EVENT_WIFI_STA_DISCONNECTED) {
        g_connect_done = true;
        g_connected = false;
    }
    return BK_OK;
}

template <size_t N>
void copy_cstr(char (&dst)[N], const char *src) {
    os_memset(dst, 0, sizeof(dst));
    if (!src) {
        return;
    }
    os_strncpy(dst, src, N - 1);
}
}

int16_t WiFiClass::_state[MAX_SOCK_NUM] = {NA_STATE, NA_STATE, NA_STATE, NA_STATE};
uint16_t WiFiClass::_server_port[MAX_SOCK_NUM] = {0, 0, 0, 0};

WiFiClass WiFi;

WiFiClass::WiFiClass()
    : m_initialized(false),
      m_station_started(false),
      m_status(WL_DISCONNECTED),
      m_connected_bssid{0},
      m_connected_rssi(0),
      m_connected_channel(0),
      m_connected_security(ENC_TYPE_NONE) {
}

uint8_t WiFiClass::getSocket() {
    return SOCK_NOT_AVAIL;
}

char *WiFiClass::firmwareVersion() {
    static char version[] = "BK";
    return version;
}

void WiFiClass::init() {
}

bool WiFiClass::ensureInitialized(void) {
    if (m_initialized) {
        return true;
    }

    if (!g_scan_cb_registered) {
        if (bk_event_register_cb(EVENT_MOD_WIFI, EVENT_WIFI_SCAN_DONE, arduino_wifi_event_cb, nullptr) != BK_OK) {
            return false;
        }
        if (bk_event_register_cb(EVENT_MOD_WIFI, EVENT_WIFI_STA_CONNECTED, arduino_wifi_event_cb, nullptr) != BK_OK) {
            return false;
        }
        if (bk_event_register_cb(EVENT_MOD_WIFI, EVENT_WIFI_STA_DISCONNECTED, arduino_wifi_event_cb, nullptr) != BK_OK) {
            return false;
        }
        g_scan_cb_registered = true;
    }

    m_initialized = true;
    return true;
}

bool WiFiClass::ensureStationStarted(void) {
    if (m_station_started) {
        return true;
    }
    if (!ensureInitialized()) {
        return false;
    }

    m_station_started = true;
    return true;
}

int WiFiClass::begin(const char *ssid) {
    return begin(ssid, "");
}

int WiFiClass::begin(const char *ssid, uint8_t, const char *key) {
    return begin(ssid, key);
}

int WiFiClass::begin(const char *ssid, const char *passphrase) {
    if (!ssid || ssid[0] == '\0') {
        m_status = WL_NO_SSID_AVAIL;
        return m_status;
    }

    if (!ensureStationStarted()) {
        m_status = WL_CONNECT_FAILED;
        return m_status;
    }

    wifi_sta_config_t config = {};
    copy_cstr(config.ssid, ssid);
    copy_cstr(config.password, passphrase ? passphrase : "");
    config.security = WIFI_SECURITY_AUTO;
    int ret = bk_wifi_sta_set_config(&config);
    if (ret != BK_OK) {
        m_status = WL_CONNECT_FAILED;
        return m_status;
    }

    g_connect_done = false;
    g_connected = false;
    ret = bk_wifi_sta_start();
    if (ret != BK_OK) {
        m_status = WL_CONNECT_FAILED;
        return m_status;
    }

    const uint32_t timeout_ms = 15000;
    const uint32_t step_ms = 200;
    for (uint32_t waited = 0; waited < timeout_ms; waited += step_ms) {
        if (g_connect_done) {
            m_status = g_connected ? WL_CONNECTED : WL_CONNECT_FAILED;
            return m_status;
        }
        delay(step_ms);
    }

    m_status = WL_CONNECT_FAILED;
    return m_status;
}

void WiFiClass::config(IPAddress) {
}

void WiFiClass::config(IPAddress, IPAddress) {
}

void WiFiClass::config(IPAddress, IPAddress, IPAddress) {
}

void WiFiClass::config(IPAddress, IPAddress, IPAddress, IPAddress) {
}

void WiFiClass::setDNS(IPAddress) {
}

void WiFiClass::setDNS(IPAddress, IPAddress) {
}

int WiFiClass::disconnect(void) {
    const bk_err_t ret = bk_wifi_sta_disconnect();
    m_status = WL_DISCONNECTED;
    return ret == BK_OK ? 1 : 0;
}

uint8_t *WiFiClass::macAddress(uint8_t *mac) {
    if (!mac) {
        return nullptr;
    }
    std::memset(mac, 0, WL_MAC_ADDR_LENGTH);
    return mac;
}

IPAddress WiFiClass::localIP() {
    return arduino::INADDR_NONE;
}

IPAddress WiFiClass::subnetMask() {
    return arduino::INADDR_NONE;
}

IPAddress WiFiClass::gatewayIP() {
    return arduino::INADDR_NONE;
}

void WiFiClass::clearScanResults(void) {
    m_scan_results.clear();
}

int8_t WiFiClass::scanNetworks() {
    if (!ensureStationStarted()) {
        return -1;
    }

    clearScanResults();
    g_scan_done = false;

    if (bk_wifi_scan_start(nullptr) != BK_OK) {
        return -1;
    }

    const uint32_t timeout_ms = 10000;
    const uint32_t step_ms = 100;
    for (uint32_t waited = 0; waited < timeout_ms; waited += step_ms) {
        if (g_scan_done) {
            wifi_scan_result_t result = {0};
            if (bk_wifi_scan_get_result(&result) != BK_OK) {
                return -1;
            }

            for (int i = 0; i < result.ap_num; ++i) {
                ScanResult ap = {};
                ap.ssid = result.aps[i].ssid;
                std::memcpy(ap.bssid, result.aps[i].bssid, sizeof(ap.bssid));
                ap.rssi = result.aps[i].rssi;
                ap.channel = result.aps[i].channel;
                ap.security = static_cast<uint8_t>(result.aps[i].security);
                m_scan_results.push_back(ap);
            }

            bk_wifi_scan_free_result(&result);
            m_status = WL_SCAN_COMPLETED;
            return static_cast<int8_t>(m_scan_results.size());
        }
        delay(step_ms);
    }

    return -1;
}
wl_status_t WiFiClass::statusFromLinkState(int state) const {
    switch (state) {
        case WIFI_LINKSTATE_STA_GOT_IP:
        case WIFI_LINKSTATE_STA_CONNECTED:
            return WL_CONNECTED;
        case WIFI_LINKSTATE_STA_CONNECTING:
            return WL_IDLE_STATUS;
        case WIFI_LINKSTATE_STA_CONNECT_FAILED:
            return WL_CONNECT_FAILED;
        case WIFI_LINKSTATE_STA_DISCONNECTED:
            return WL_DISCONNECTED;
        case WIFI_LINKSTATE_STA_IDLE:
        default:
            return WL_DISCONNECTED;
    }
}

uint8_t WiFiClass::status(void) {
    if (!ensureInitialized()) {
        m_status = WL_CONNECT_FAILED;
        return m_status;
    }

    wifi_link_status_t link = {};
    if (bk_wifi_sta_get_link_status(&link) != BK_OK) {
        m_status = WL_DISCONNECTED;
        return m_status;
    }

    m_status = statusFromLinkState(link.state);
    if (m_status == WL_CONNECTED) {
        m_connected_ssid = link.ssid;
        std::memcpy(m_connected_bssid, link.bssid, sizeof(m_connected_bssid));
        m_connected_rssi = link.rssi;
        m_connected_channel = link.channel;
        m_connected_security = static_cast<uint8_t>(link.security);
    }
    return m_status;
}

char *WiFiClass::SSID() {
    return const_cast<char *>(m_connected_ssid.c_str());
}

char *WiFiClass::SSID(uint8_t networkItem) {
    if (static_cast<size_t>(networkItem) >= m_scan_results.size()) {
        return nullptr;
    }
    return const_cast<char *>(m_scan_results[static_cast<size_t>(networkItem)].ssid.c_str());
}

uint8_t *WiFiClass::BSSID(uint8_t *bssid) {
    if (!bssid) {
        return nullptr;
    }
    std::memcpy(bssid, m_connected_bssid, sizeof(m_connected_bssid));
    return bssid;
}

int32_t WiFiClass::RSSI() {
    return m_connected_rssi;
}

int32_t WiFiClass::RSSI(uint8_t networkItem) {
    if (static_cast<size_t>(networkItem) >= m_scan_results.size()) {
        return 0;
    }
    return m_scan_results[static_cast<size_t>(networkItem)].rssi;
}

uint8_t WiFiClass::encryptionType() {
    return m_connected_security;
}

uint8_t WiFiClass::encryptionType(uint8_t networkItem) {
    if (static_cast<size_t>(networkItem) >= m_scan_results.size()) {
        return 0;
    }
    return m_scan_results[static_cast<size_t>(networkItem)].security;
}

int WiFiClass::hostByName(const char *, IPAddress &aResult) {
    aResult = arduino::INADDR_NONE;
    return 0;
}