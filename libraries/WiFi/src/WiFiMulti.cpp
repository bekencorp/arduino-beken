#include "WiFiMulti.h"
#include "WiFiBkIdk.h"

extern "C" {
#include <limits.h>
#include <os/mem.h>
#include <os/os.h>
#include <string.h>
}

WiFiMulti::WiFiMulti() = default;

WiFiMulti::~WiFiMulti() {
  for (size_t i = 0; i < _ap_count; i++) {
    if (_aplist[i].ssid != nullptr) {
      os_free(_aplist[i].ssid);
    }
    if (_aplist[i].passphrase != nullptr) {
      os_free(_aplist[i].passphrase);
    }
  }
  _ap_count = 0;
}

bool WiFiMulti::addAP(const char *ssid, const char *passphrase) {
  if (ssid == nullptr || ssid[0] == '\0' || strlen(ssid) > 31 || _ap_count >= kMaxAps) {
    return false;
  }
  if (passphrase != nullptr && strlen(passphrase) > 64) {
    return false;
  }

  WifiAPlist_t &entry = _aplist[_ap_count];
  entry.ssid = static_cast<char *>(os_malloc(strlen(ssid) + 1));
  if (entry.ssid == nullptr) {
    return false;
  }
  strcpy(entry.ssid, ssid);

  if (passphrase != nullptr && passphrase[0] != '\0') {
    entry.passphrase = static_cast<char *>(os_malloc(strlen(passphrase) + 1));
    if (entry.passphrase == nullptr) {
      os_free(entry.ssid);
      entry.ssid = nullptr;
      return false;
    }
    strcpy(entry.passphrase, passphrase);
  } else {
    entry.passphrase = nullptr;
  }

  _ap_count++;
  return true;
}

uint8_t WiFiMulti::run(uint32_t connectTimeout) {
  WF_STATION_STAT_E sta_status = WiFi.status();
  wl_status_t status = wifiBkIdkToWlStatus(sta_status);

  if (status == WL_CONNECTED) {
    for (size_t x = 0; x < _ap_count; x++) {
      if (WiFi.SSID() == _aplist[x].ssid) {
        return status;
      }
    }
    WiFi.disconnect(false, false);
    rtos_delay_milliseconds(10);
    sta_status = WiFi.status();
    status = wifiBkIdkToWlStatus(sta_status);
  }

  const int16_t scanResult = WiFi.scanNetworks();
  if (scanResult == WIFI_SCAN_RUNNING) {
    return WL_NO_SSID_AVAIL;
  }

  if (scanResult >= 0) {
    WifiAPlist_t bestNetwork = {nullptr, nullptr};
    int bestNetworkDb = INT_MIN;
    uint8_t bestBSSID[6] = {};
    int32_t bestChannel = 0;

    if (scanResult > 0) {
      for (int8_t i = 0; i < scanResult; ++i) {
        String ssid_scan;
        int32_t rssi_scan;
        uint8_t sec_scan;
        uint8_t *bssid_scan;
        int32_t chan_scan;

        if (!WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, bssid_scan, chan_scan)) {
          continue;
        }

        for (size_t x = _ap_count; x > 0; x--) {
          const WifiAPlist_t &entry = _aplist[x - 1];
          if (ssid_scan == entry.ssid) {
            if (rssi_scan > bestNetworkDb) {
              if (sec_scan == WAAM_OPEN || entry.passphrase != nullptr) {
                bestNetworkDb = rssi_scan;
                bestChannel = chan_scan;
                bestNetwork = entry;
                memcpy(bestBSSID, bssid_scan, sizeof(bestBSSID));
              }
            }
            break;
          }
        }
      }
    }

    WiFi.scanDelete();

    if (bestNetwork.ssid != nullptr) {
      WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID);
      const uint32_t startTime = millis();
      sta_status = WiFi.status();
      status = wifiBkIdkToWlStatus(sta_status);

      while (status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED &&
             (millis() - startTime) <= connectTimeout) {
        rtos_delay_milliseconds(10);
        sta_status = WiFi.status();
        status = wifiBkIdkToWlStatus(sta_status);
      }
    }
  } else {
    WiFi.disconnect();
    WiFi.scanNetworks(true);
  }

  return status;
}
