#include "WiFiSTA.h"
#include "WiFiGeneric.h"
#include "WiFiBkIdk.h"

extern "C" {
#include <common/bk_err.h>
#include <components/netif.h>
#include <components/netif_types.h>
#include <modules/wifi.h>
#include <modules/wifi_types.h>
#include <os/mem.h>
#include <os/os.h>
#include <os/str.h>
}

bool WiFiSTAClass::_useStaticIp = false;
bool WiFiSTAClass::_autoReconnect = true;
String WiFiSTAClass::_savedSsid;
static netif_ip4_config_t s_staticIpConfig = {};

static void copyIpToNetifConfig(const IPAddress &ip, char *dest, size_t len) {
  String s = ip.toString();
  os_strlcpy(dest, s.c_str(), len);
}

void WiFiSTAClass::applyStaticIpIfConfigured() {
  if (!_useStaticIp) {
    return;
  }
  // Sets sta_static_ip_flag so netif_wifi_event_cb applies this on STA_CONNECTED.
  (void)bk_netif_static_ip(s_staticIpConfig);
}

WF_STATION_STAT_E WiFiSTAClass::begin(const char *, wpa2_auth_method_t, const char *, const char *,
                                      const char *, const char *, const char *, const char *,
                                      int32_t, const uint8_t *, bool) {
  return status();
}

WF_STATION_STAT_E WiFiSTAClass::begin(const char *ssid, const char *passphrase, int32_t channel,
                                      const uint8_t *bssid, bool connect) {
  if (!WiFiGenericClass::enableSTA(true)) {
    return WSS_CONN_FAIL;
  }

  if (ssid == nullptr || ssid[0] == '\0' || os_strlen(ssid) > 32) {
    return WSS_CONN_FAIL;
  }
  if (passphrase != nullptr && os_strlen(passphrase) > 64) {
    return WSS_CONN_FAIL;
  }

  wifi_sta_config_t sta_config = {};
  os_strlcpy(sta_config.ssid, ssid, sizeof(sta_config.ssid));
  if (passphrase != nullptr) {
    os_strlcpy(sta_config.password, passphrase, sizeof(sta_config.password));
  }
  sta_config.security = WIFI_SECURITY_AUTO;
  if (channel > 0) {
    sta_config.channel = static_cast<uint8_t>(channel);
  }
  if (bssid != nullptr) {
    os_memcpy(sta_config.bssid, bssid, WIFI_BSSID_LEN);
  }

  bk_wifi_sta_stop();

  if (bk_wifi_sta_set_config(&sta_config) != BK_OK) {
    return WSS_CONN_FAIL;
  }

  _savedSsid = ssid;

  if (!connect) {
    return status();
  }

  // Re-arm before start: a prior disconnect clears SDK static-IP flag.
  applyStaticIpIfConfigured();

  if (bk_wifi_sta_start() != BK_OK) {
    return WSS_CONN_FAIL;
  }

  WiFiGenericClass::dispatchEvent(ARDUINO_EVENT_WIFI_STA_START);

  return status();
}

WF_STATION_STAT_E WiFiSTAClass::begin(char *ssid, char *passphrase, int32_t channel,
                                      const uint8_t *bssid, bool connect) {
  return begin(const_cast<const char *>(ssid), passphrase, channel, bssid, connect);
}

WF_STATION_STAT_E WiFiSTAClass::begin() {
  return status();
}

bool WiFiSTAClass::reconnect() {
  applyStaticIpIfConfigured();
  return bk_wifi_sta_connect() == BK_OK;
}

bool WiFiSTAClass::disconnect(bool wifioff, bool) {
  bk_wifi_sta_disconnect();
  if (wifioff) {
    bk_wifi_sta_stop();
    WiFiGenericClass::dispatchEvent(ARDUINO_EVENT_WIFI_STA_STOP);
    WiFiGenericClass::_mode = WIFI_OFF;
  }
  return true;
}

bool WiFiSTAClass::eraseAP(void) {
  return true;
}

bool WiFiSTAClass::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1,
                          IPAddress) {
  if (!WiFiGenericClass::enableSTA(true)) {
    return false;
  }

  // Need DISCONNECTED handler to re-arm static IP after SDK clears its flag.
  WiFiGenericClass::ensureEventHandlersRegistered();

  netif_ip4_config_t cfg = {};
  copyIpToNetifConfig(local_ip, cfg.ip, sizeof(cfg.ip));
  copyIpToNetifConfig(gateway, cfg.gateway, sizeof(cfg.gateway));
  copyIpToNetifConfig(subnet, cfg.mask, sizeof(cfg.mask));
  if (dns1 != IPAddress((uint32_t)0)) {
    copyIpToNetifConfig(dns1, cfg.dns, sizeof(cfg.dns));
  } else {
    // Fall back to gateway as DNS if caller omitted dns1.
    copyIpToNetifConfig(gateway, cfg.dns, sizeof(cfg.dns));
  }

  s_staticIpConfig = cfg;
  _useStaticIp = true;
  // Do not rely on set_ip4_config alone: STA still starts DHCP unless the
  // Beken static-IP flag is set via bk_netif_static_ip() before association.
  return bk_netif_static_ip(s_staticIpConfig) == BK_OK;
}

bool WiFiSTAClass::isConnected() {
  return status() == WSS_GOT_IP;
}

bool WiFiSTAClass::setAutoConnect(bool) {
  return false;
}

bool WiFiSTAClass::getAutoConnect() {
  return false;
}

bool WiFiSTAClass::setAutoReconnect(bool autoReconnect) {
  _autoReconnect = autoReconnect;
  return true;
}

bool WiFiSTAClass::getAutoReconnect() {
  return _autoReconnect;
}

uint8_t WiFiSTAClass::waitForConnectResult(unsigned long timeoutLength) {
  if ((WiFiGenericClass::getMode() & WIFI_STA) == 0) {
    return WSS_CONN_FAIL;
  }

  const uint32_t start = rtos_get_time();
  while ((rtos_get_time() - start) < timeoutLength) {
    WF_STATION_STAT_E st = status();
    if (st == WSS_GOT_IP || st == WSS_CONN_FAIL || st == WSS_NO_AP_FOUND ||
        st == WSS_PASSWD_WRONG || st == WSS_DHCP_FAIL) {
      return st;
    }
    rtos_delay_milliseconds(100);
  }
  return status();
}

IPAddress WiFiSTAClass::localIP() {
  uint32_t address = 0;
  if (wifiBkIdkGetStaNetifIpv4(&address, nullptr, nullptr, nullptr)) {
    return wifiBkIdkIpFromU32(address);
  }
  return IPAddress();
}

uint8_t *WiFiSTAClass::macAddress(uint8_t *mac) {
  if (mac != nullptr) {
    bk_wifi_sta_get_mac(mac);
  }
  return mac;
}

String WiFiSTAClass::macAddress() {
  uint8_t mac[6] = {};
  bk_wifi_sta_get_mac(mac);
  char macStr[18] = {};
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);
  return String(macStr);
}

IPAddress WiFiSTAClass::subnetMask() {
  uint32_t netmask = 0;
  if (wifiBkIdkGetStaNetifIpv4(nullptr, &netmask, nullptr, nullptr)) {
    return wifiBkIdkIpFromU32(netmask);
  }
  return IPAddress();
}

IPAddress WiFiSTAClass::gatewayIP() {
  uint32_t gateway = 0;
  if (wifiBkIdkGetStaNetifIpv4(nullptr, nullptr, &gateway, nullptr)) {
    return wifiBkIdkIpFromU32(gateway);
  }
  return IPAddress();
}

IPAddress WiFiSTAClass::dnsIP(uint8_t) {
  uint32_t dns = 0;
  if (wifiBkIdkGetStaNetifIpv4(nullptr, nullptr, nullptr, &dns)) {
    return wifiBkIdkIpFromU32(dns);
  }
  return IPAddress();
}

IPAddress WiFiSTAClass::broadcastIP() {
  return WiFiGenericClass::calculateBroadcast(localIP(), subnetMask());
}

IPAddress WiFiSTAClass::networkID() {
  return WiFiGenericClass::calculateNetworkID(localIP(), subnetMask());
}

uint8_t WiFiSTAClass::subnetCIDR() {
  return WiFiGenericClass::calculateSubnetCIDR(subnetMask());
}

WF_STATION_STAT_E WiFiSTAClass::status() {
  return wifiBkIdkPollStaStatus();
}

String WiFiSTAClass::SSID() const {
  wifi_link_status_t link = {};
  if (bk_wifi_sta_get_link_status(&link) == BK_OK && link.ssid[0] != '\0') {
    return String(link.ssid);
  }
  return _savedSsid;
}

String WiFiSTAClass::psk() const {
  return String();
}

uint8_t *WiFiSTAClass::BSSID() {
  static uint8_t bssid[WIFI_BSSID_LEN] = {};
  wifi_link_status_t link = {};
  if (bk_wifi_sta_get_link_status(&link) == BK_OK) {
    os_memcpy(bssid, link.bssid, WIFI_BSSID_LEN);
  }
  return bssid;
}

String WiFiSTAClass::BSSIDstr() {
  uint8_t *bssid = BSSID();
  char bssidStr[18] = {};
  snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1],
           bssid[2], bssid[3], bssid[4], bssid[5]);
  return String(bssidStr);
}

int8_t WiFiSTAClass::RSSI() {
  wifi_link_status_t link = {};
  if (bk_wifi_sta_get_link_status(&link) == BK_OK) {
    return static_cast<int8_t>(link.rssi);
  }
  return 0;
}
