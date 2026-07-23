#ifndef WIFIMULTI_H_
#define WIFIMULTI_H_

#include "WiFi.h"

typedef struct {
  char *ssid;
  char *passphrase;
} WifiAPlist_t;

class WiFiMulti {
public:
  WiFiMulti();
  ~WiFiMulti();

  bool addAP(const char *ssid, const char *passphrase = nullptr);
  uint8_t run(uint32_t connectTimeout = 5000);

private:
  static constexpr size_t kMaxAps = 10;
  WifiAPlist_t _aplist[kMaxAps] = {};
  size_t _ap_count = 0;
};

#endif
