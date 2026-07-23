#ifndef WIFIAP_H_
#define WIFIAP_H_

#include <Arduino.h>
#include "WiFiType.h"
#include "api/IPAddress.h"

class WiFiAPClass {
public:
  bool softAP(const char *ssid, const char *passphrase = nullptr, int channel = 1,
            int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false);
  bool softAP(const String &ssid, const String &passphrase = (const char *)nullptr, int channel = 1,
            int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false) {
    return softAP(ssid.c_str(), passphrase.c_str(), channel, ssid_hidden, max_connection,
                  ftm_responder);
  }

  bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet,
                    IPAddress dhcp_lease_start = (uint32_t)0);
  bool softAPdisconnect(bool wifioff = false);
  uint8_t softAPgetStationNum();
  IPAddress softAPIP();
  IPAddress softAPBroadcastIP();
  IPAddress softAPNetworkID();
  IPAddress softAPSubnetMask();
  uint8_t softAPSubnetCIDR();
  uint8_t *softAPmacAddress(uint8_t *mac);
  String softAPmacAddress(void);
  String softAPSSID(void) const;
  String softAPPSK(void) const;
};

#endif
