#ifndef WiFi_h
#define WiFi_h

#include <Arduino.h>
#include "WiFiType.h"
#include "WiFiGeneric.h"
#include "WiFiSTA.h"
#include "WiFiScan.h"
#include "WiFiAP.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiUdp.h"

class WiFiClass : public WiFiGenericClass, public WiFiSTAClass, public WiFiScanClass,
                  public WiFiAPClass {
public:
  using WiFiGenericClass::channel;

  using WiFiSTAClass::SSID;
  using WiFiSTAClass::RSSI;
  using WiFiSTAClass::BSSID;
  using WiFiSTAClass::BSSIDstr;

  using WiFiScanClass::SSID;
  using WiFiScanClass::RSSI;
  using WiFiScanClass::BSSID;
  using WiFiScanClass::BSSIDstr;
  using WiFiScanClass::channel;

  void printDiag(Print &dest);
};

extern WiFiClass WiFi;

#endif
