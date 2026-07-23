#include "WiFiGeneric.h"
#include "WiFiBkIdk.h"
#include "WiFiSTA.h"

#include <vector>

extern "C" {
#include <common/bk_err.h>
#include <components/event.h>
#include <components/netif_types.h>
#include <lwip/dns.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <modules/wifi.h>
#include <modules/wifi_types.h>
#include <os/os.h>
}

bool WiFiGenericClass::_persistent = true;
bool WiFiGenericClass::_long_range = false;
bool WiFiGenericClass::_wifiUseStaticBuffers = false;
WiFiMode_t WiFiGenericClass::_mode = WIFI_OFF;

namespace {
struct WiFiEventHandlerEntry {
  WiFiCallbackId id;
  WiFiEventCb cb;
  arduino_event_id_t event;
};

std::vector<WiFiEventHandlerEntry> g_wifiEventHandlers;
WiFiCallbackId g_nextWiFiEventId = 1;
bool g_wifiEventsRegistered = false;

bk_err_t wifiGenericWifiEventCb(void *, event_module_t, int event_id, void *) {
  switch (event_id) {
    case EVENT_WIFI_SCAN_DONE:
      WiFiGenericClass::dispatchEvent(ARDUINO_EVENT_WIFI_SCAN_DONE);
      break;
    case EVENT_WIFI_STA_CONNECTED:
      // netif_wifi_event_cb (registered earlier by bk_netif_init) applies the
      // static IP when sta_static_ip_flag is set. Re-arm is done on disconnect.
      WiFiGenericClass::dispatchEvent(ARDUINO_EVENT_WIFI_STA_CONNECTED);
      break;
    case EVENT_WIFI_STA_DISCONNECTED:
      // SDK netif clears sta_static_ip_flag on disconnect; re-arm for the next
      // association so static IP survives reconnect.
      WiFiSTAClass::applyStaticIpIfConfigured();
      WiFiGenericClass::dispatchEvent(ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
      break;
    default:
      break;
  }
  return BK_OK;
}

bk_err_t wifiGenericNetifEventCb(void *, event_module_t, int event_id, void *event_data) {
  if (event_id != EVENT_NETIF_GOT_IP4 || event_data == nullptr) {
    return BK_OK;
  }

  netif_event_got_ip4_t *got_ip = static_cast<netif_event_got_ip4_t *>(event_data);
  if (got_ip->netif_if == NETIF_IF_STA) {
    WiFiGenericClass::dispatchEvent(ARDUINO_EVENT_WIFI_STA_GOT_IP);
  }
  return BK_OK;
}
}  // namespace

int WiFiGenericClass::getStatusBits() {
  return 0;
}

int WiFiGenericClass::waitStatusBits(int, uint32_t) {
  return 0;
}

void WiFiGenericClass::ensureEventHandlersRegistered() {
  if (g_wifiEventsRegistered) {
    return;
  }

  if (bk_event_register_cb(EVENT_MOD_WIFI, EVENT_ID_ALL, wifiGenericWifiEventCb, nullptr) != BK_OK) {
    return;
  }
  if (bk_event_register_cb(EVENT_MOD_NETIF, EVENT_ID_ALL, wifiGenericNetifEventCb, nullptr) != BK_OK) {
    bk_event_unregister_cb(EVENT_MOD_WIFI, EVENT_ID_ALL, wifiGenericWifiEventCb);
    return;
  }

  g_wifiEventsRegistered = true;
}

void WiFiGenericClass::dispatchEvent(arduino_event_id_t event) {
  for (const WiFiEventHandlerEntry &entry : g_wifiEventHandlers) {
    if (entry.cb == nullptr) {
      continue;
    }
    if (entry.event == ARDUINO_EVENT_MAX || entry.event == event) {
      entry.cb(event);
    }
  }
}

WiFiCallbackId WiFiGenericClass::onEvent(WiFiEventCb cbEvent, arduino_event_id_t event) {
  if (cbEvent == nullptr) {
    return 0;
  }

  ensureEventHandlersRegistered();

  const WiFiCallbackId id = g_nextWiFiEventId++;
  g_wifiEventHandlers.push_back({id, cbEvent, event});
  return id;
}

void WiFiGenericClass::removeEvent(WiFiEventCb cbEvent, arduino_event_id_t event) {
  if (cbEvent == nullptr) {
    return;
  }

  for (auto it = g_wifiEventHandlers.begin(); it != g_wifiEventHandlers.end();) {
    if (it->cb == cbEvent && it->event == event) {
      it = g_wifiEventHandlers.erase(it);
    } else {
      ++it;
    }
  }
}

void WiFiGenericClass::removeEvent(WiFiCallbackId id) {
  for (auto it = g_wifiEventHandlers.begin(); it != g_wifiEventHandlers.end();) {
    if (it->id == id) {
      it = g_wifiEventHandlers.erase(it);
    } else {
      ++it;
    }
  }
}

int32_t WiFiGenericClass::channel(void) {
  wifi_link_status_t link = {};
  if (bk_wifi_sta_get_link_status(&link) == BK_OK) {
    return link.channel;
  }
  return 0;
}

void WiFiGenericClass::persistent(bool persistent) {
  _persistent = persistent;
}

void WiFiGenericClass::enableLongRange(bool enable) {
  _long_range = enable;
}

bool WiFiGenericClass::mode(WiFiMode_t m) {
  if (m == WIFI_OFF) {
    if ((_mode & WIFI_STA) != 0) {
      bk_wifi_sta_stop();
    }
    if ((_mode & WIFI_AP) != 0) {
      bk_wifi_ap_stop();
    }
    _mode = WIFI_OFF;
    return true;
  }

  bool ok = true;
  ok &= enableSTA((m & WIFI_STA) != 0);
  ok &= enableAP((m & WIFI_AP) != 0);
  return ok;
}

WiFiMode_t WiFiGenericClass::getMode() {
  return _mode;
}

bool WiFiGenericClass::enableSTA(bool enable) {
  if (enable) {
    _mode = static_cast<WiFiMode_t>(_mode | WIFI_STA);
    if (_mode == WIFI_OFF) {
      _mode = WIFI_STA;
    }
    return true;
  }
  if ((_mode & WIFI_STA) != 0) {
    bk_wifi_sta_stop();
    _mode = static_cast<WiFiMode_t>(_mode & ~WIFI_STA);
    if (_mode == WIFI_OFF) {
      return true;
    }
  }
  return true;
}

bool WiFiGenericClass::enableAP(bool enable) {
  if (enable) {
    _mode = static_cast<WiFiMode_t>(_mode | WIFI_AP);
    return true;
  }
  if ((_mode & WIFI_AP) != 0) {
    bk_wifi_ap_stop();
    _mode = static_cast<WiFiMode_t>(_mode & ~WIFI_AP);
  }
  return true;
}

bool WiFiGenericClass::setSleep(bool) {
  return false;
}

bool WiFiGenericClass::setDualAntennaConfig(uint8_t, uint8_t, int, int) {
  return false;
}

void WiFiGenericClass::useStaticBuffers(bool bufferMode) {
  _wifiUseStaticBuffers = bufferMode;
}

bool WiFiGenericClass::useStaticBuffers() {
  return _wifiUseStaticBuffers;
}

int WiFiGenericClass::hostByName(const char *aHostname, IPAddress &aResult) {
  if (aHostname == nullptr) {
    return 0;
  }

  IPAddress parsed;
  if (parsed.fromString(aHostname)) {
    aResult = parsed;
    return 1;
  }

  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo *result = nullptr;
  if (lwip_getaddrinfo(aHostname, nullptr, &hints, &result) != 0 || result == nullptr) {
    return 0;
  }

  struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(result->ai_addr);
  aResult = static_cast<uint32_t>(addr->sin_addr.s_addr);
  lwip_freeaddrinfo(result);
  return 1;
}

IPAddress WiFiGenericClass::calculateNetworkID(IPAddress ip, IPAddress subnet) {
  IPAddress network;
  for (int i = 0; i < 4; i++) {
    network[i] = ip[i] & subnet[i];
  }
  return network;
}

IPAddress WiFiGenericClass::calculateBroadcast(IPAddress ip, IPAddress subnet) {
  IPAddress broadcast;
  for (int i = 0; i < 4; i++) {
    broadcast[i] = ip[i] | static_cast<uint8_t>(~subnet[i]);
  }
  return broadcast;
}

uint8_t WiFiGenericClass::calculateSubnetCIDR(IPAddress subnetMask) {
  uint32_t mask = (static_cast<uint32_t>(subnetMask[0]) << 24) |
                  (static_cast<uint32_t>(subnetMask[1]) << 16) |
                  (static_cast<uint32_t>(subnetMask[2]) << 8) |
                  static_cast<uint32_t>(subnetMask[3]);
  uint8_t cidr = 0;
  while (mask) {
    cidr += mask & 1;
    mask >>= 1;
  }
  return cidr;
}
