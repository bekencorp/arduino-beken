#include "WiFiAP.h"
#include "WiFiGeneric.h"
#include "WiFiBkIdk.h"

extern "C" {
#include <common/bk_err.h>
#include <components/netif.h>
#include <components/netif_types.h>
#include <modules/wifi.h>
#include <modules/wifi_types.h>
#include <os/str.h>
#include <string.h>
}

// Tracks whether the user provided a custom AP IP via softAPConfig(). When not
// set, softAP() installs a default IP config so the DHCP server hands out
// addresses in a valid subnet (matching the SDK CLI which sets 192.168.0.1).
static bool s_apIpConfigured = false;

static void copyIpToNetifConfig(const IPAddress &ip, char *dest, size_t len) {
  String s = ip.toString();
  os_strlcpy(dest, s.c_str(), len);
}

static bool applyDefaultApIpConfig() {
  const IPAddress apIp(192, 168, 0, 1);
  const IPAddress apMask(255, 255, 255, 0);
  netif_ip4_config_t config = {};
  copyIpToNetifConfig(apIp, config.ip, sizeof(config.ip));
  copyIpToNetifConfig(apIp, config.gateway, sizeof(config.gateway));
  copyIpToNetifConfig(apMask, config.mask, sizeof(config.mask));
  os_strlcpy(config.dns, "0.0.0.0", sizeof(config.dns));
  return bk_netif_set_ip4_config(NETIF_IF_AP, &config) == BK_OK;
}

bool WiFiAPClass::softAP(const char *ssid, const char *passphrase, int channel, int ssid_hidden,
                         int max_connection, bool) {
  if (ssid == nullptr || ssid[0] == '\0') {
    return false;
  }
  if (passphrase != nullptr && strlen(passphrase) > 0 && strlen(passphrase) < 8) {
    return false;
  }
  if (!WiFiGenericClass::enableAP(true)) {
    return false;
  }

  // The AP netif must have a valid IP before the DHCP server starts, otherwise
  // connecting stations fail to obtain an address. Install a default unless the
  // sketch already called softAPConfig().
  if (!s_apIpConfigured) {
    applyDefaultApIpConfig();
  }

  wifi_ap_config_t ap_config = {};
  os_strlcpy(ap_config.ssid, "ap_default_ssid", sizeof(ap_config.ssid));
  ap_config.security = WIFI_SECURITY_WPA2_MIXED;
  os_strlcpy(ap_config.ssid, ssid, sizeof(ap_config.ssid));
  if (passphrase != nullptr && passphrase[0] != '\0') {
    os_strlcpy(ap_config.password, passphrase, sizeof(ap_config.password));
    ap_config.security = WIFI_SECURITY_WPA2_MIXED;
  } else {
    ap_config.password[0] = '\0';
    ap_config.security = WIFI_SECURITY_NONE;
  }
  if (channel > 0) {
    ap_config.channel = static_cast<uint8_t>(channel);
  }
  ap_config.hidden = ssid_hidden ? 1 : 0;
  if (max_connection > 0) {
    ap_config.max_con = static_cast<uint8_t>(max_connection);
  }

  if (bk_wifi_ap_set_config(&ap_config) != BK_OK) {
    return false;
  }
  return bk_wifi_ap_start() == BK_OK;
}

String WiFiAPClass::softAPSSID() const {
  wifi_ap_config_t info = {};
  if (bk_wifi_ap_get_config(&info) == BK_OK) {
    return String(info.ssid);
  }
  return String();
}

String WiFiAPClass::softAPPSK() const {
  wifi_ap_config_t info = {};
  if (bk_wifi_ap_get_config(&info) == BK_OK) {
    return String(info.password);
  }
  return String();
}

bool WiFiAPClass::softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress) {
  if (!WiFiGenericClass::enableAP(true)) {
    return false;
  }

  netif_ip4_config_t config = {};
  copyIpToNetifConfig(local_ip, config.ip, sizeof(config.ip));
  copyIpToNetifConfig(gateway, config.gateway, sizeof(config.gateway));
  copyIpToNetifConfig(subnet, config.mask, sizeof(config.mask));
  // Advertise AP IP as DNS so SoftAP clients send queries to this board (captive portal).
  copyIpToNetifConfig(local_ip, config.dns, sizeof(config.dns));
  if (bk_netif_set_ip4_config(NETIF_IF_AP, &config) != BK_OK) {
    return false;
  }
  s_apIpConfigured = true;
  return true;
}

bool WiFiAPClass::softAPdisconnect(bool wifioff) {
  bk_wifi_ap_stop();
  s_apIpConfigured = false;
  if (wifioff) {
    return WiFiGenericClass::enableAP(false);
  }
  return true;
}

uint8_t WiFiAPClass::softAPgetStationNum() {
  wlan_ap_stas_t clients = {};
  if (bk_wifi_ap_get_sta_list(&clients) == BK_OK) {
    return clients.num;
  }
  return 0;
}

IPAddress WiFiAPClass::softAPIP() {
  uint32_t address = 0;
  if (wifiBkIdkGetApNetifIpv4(&address, nullptr, nullptr)) {
    return wifiBkIdkIpFromU32(address);
  }
  return IPAddress();
}

IPAddress WiFiAPClass::softAPBroadcastIP() {
  return WiFiGenericClass::calculateBroadcast(softAPIP(), softAPSubnetMask());
}

IPAddress WiFiAPClass::softAPNetworkID() {
  return WiFiGenericClass::calculateNetworkID(softAPIP(), softAPSubnetMask());
}

IPAddress WiFiAPClass::softAPSubnetMask() {
  uint32_t netmask = 0;
  if (wifiBkIdkGetApNetifIpv4(nullptr, &netmask, nullptr)) {
    return wifiBkIdkIpFromU32(netmask);
  }
  return IPAddress();
}

uint8_t WiFiAPClass::softAPSubnetCIDR() {
  return WiFiGenericClass::calculateSubnetCIDR(softAPSubnetMask());
}

uint8_t *WiFiAPClass::softAPmacAddress(uint8_t *mac) {
  if (mac != nullptr) {
    bk_wifi_ap_get_mac(mac);
  }
  return mac;
}

String WiFiAPClass::softAPmacAddress(void) {
  uint8_t mac[6] = {};
  bk_wifi_ap_get_mac(mac);
  char macStr[18] = {};
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
           mac[4], mac[5]);
  return String(macStr);
}
