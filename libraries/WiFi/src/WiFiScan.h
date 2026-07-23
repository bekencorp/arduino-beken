#ifndef WIFISCAN_H_
#define WIFISCAN_H_

#include <Arduino.h>
#include "WiFiType.h"

class WiFiScanClass {
public:
  int16_t scanNetworks(bool async = false, bool show_hidden = false, bool passive = false,
                       uint32_t max_ms_per_chan = 300, uint8_t channel = 0,
                       const char *ssid = nullptr, const uint8_t *bssid = nullptr);
  void scanDelete();
  bool getNetworkInfo(uint32_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &rssi,
                      uint8_t *&bssidOut, int32_t &channelOut);

  String SSID(uint8_t networkItem);
  WF_AP_AUTH_MODE_E encryptionType(uint8_t networkItem);
  int32_t RSSI(uint8_t networkItem);
  uint8_t *BSSID(uint8_t networkItem);
  String BSSIDstr(uint8_t networkItem);
  int32_t channel(uint8_t networkItem);

protected:
  const void *_getScanInfoByIndex(uint32_t i) const;

private:
  struct ScanApEntry {
    char ssid[33];
    uint8_t bssid[6];
    int rssi;
    uint8_t channel;
    uint8_t security;
  };

  ScanApEntry *_aps = nullptr;
  uint32_t _ap_num = 0;
};

#endif
