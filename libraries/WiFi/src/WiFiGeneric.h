#ifndef WIFIGENERIC_H_
#define WIFIGENERIC_H_

#include <Arduino.h>
#include "WiFiType.h"
#include "api/IPAddress.h"

typedef void (*WiFiEventCb)(arduino_event_id_t event);
typedef void (*WiFiEventSysCb)(void *event);

class WiFiGenericClass {
public:
  WiFiCallbackId onEvent(WiFiEventCb cbEvent, arduino_event_id_t event = ARDUINO_EVENT_MAX);
  void removeEvent(WiFiEventCb cbEvent, arduino_event_id_t event = ARDUINO_EVENT_MAX);
  void removeEvent(WiFiCallbackId id);

  static int getStatusBits();
  static int waitStatusBits(int bits, uint32_t timeout_ms);
  static void ensureEventHandlersRegistered();
  static void dispatchEvent(arduino_event_id_t event);

  int32_t channel(void);

  void persistent(bool persistent);
  void enableLongRange(bool enable);

  static bool mode(WiFiMode_t m);
  static WiFiMode_t getMode();

  static bool enableSTA(bool enable);
  static bool enableAP(bool enable);

  bool setSleep(bool enabled);
  static bool setDualAntennaConfig(uint8_t gpio_ant1, uint8_t gpio_ant2, int rx_mode, int tx_mode);

  static void useStaticBuffers(bool bufferMode);
  static bool useStaticBuffers();

  static int hostByName(const char *aHostname, IPAddress &aResult);

  static IPAddress calculateNetworkID(IPAddress ip, IPAddress subnet);
  static IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);
  static uint8_t calculateSubnetCIDR(IPAddress subnetMask);

protected:
  static bool _persistent;
  static bool _long_range;
  static bool _wifiUseStaticBuffers;
  static WiFiMode_t _mode;

  friend class WiFiSTAClass;
  friend class WiFiScanClass;
};

#endif
