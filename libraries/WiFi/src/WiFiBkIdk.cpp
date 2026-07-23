#include "WiFiBkIdk.h"

extern "C" {
#include <common/bk_err.h>
#include <components/netif.h>
#include <components/netif_types.h>
#include <lwip/ip4_addr.h>
#include <modules/wifi.h>
#include <modules/wifi_types.h>
#include <os/str.h>
#include "net.h"
}

static bool wifiBkIdkHasValidStaIp(const netif_ip4_config_t *ip4) {
  if (ip4 == nullptr || ip4->ip[0] == '\0') {
    return false;
  }
  return os_strcmp(ip4->ip, "0.0.0.0") != 0;
}

WF_STATION_STAT_E wifiBkIdkMapLinkState(int link_state, int reason_code) {
  switch (static_cast<wifi_link_state_t>(link_state)) {
    case WIFI_LINKSTATE_STA_IDLE:
    case WIFI_LINKSTATE_STA_DISCONNECTED:
      return WSS_IDLE;
    case WIFI_LINKSTATE_STA_CONNECTING:
      return WSS_CONNECTING;
    case WIFI_LINKSTATE_STA_CONNECTED:
      return WSS_CONN_SUCCESS;
    case WIFI_LINKSTATE_STA_GOT_IP:
      return WSS_GOT_IP;
    case WIFI_LINKSTATE_STA_CONNECT_FAILED:
      if (reason_code == WIFI_REASON_NO_AP_FOUND) {
        return WSS_NO_AP_FOUND;
      }
      if (reason_code == WIFI_REASON_WRONG_PASSWORD) {
        return WSS_PASSWD_WRONG;
      }
      if (reason_code == WIFI_REASON_DHCP_TIMEOUT) {
        return WSS_DHCP_FAIL;
      }
      return WSS_CONN_FAIL;
    case WIFI_LINKSTATE_STA_SCAN_DONE:
      return WSS_IDLE;
    default:
      return WSS_IDLE;
  }
}

WF_STATION_STAT_E wifiBkIdkPollStaStatus() {
  wifi_linkstate_reason_t info = {};
  if (bk_wifi_sta_get_linkstate_with_reason(&info) == BK_OK) {
    WF_STATION_STAT_E mapped = wifiBkIdkMapLinkState(info.state, info.reason_code);
    if (mapped == WSS_CONN_SUCCESS || mapped == WSS_GOT_IP) {
      netif_ip4_config_t ip4 = {};
      if (bk_netif_get_ip4_config(NETIF_IF_STA, &ip4) == BK_OK && wifiBkIdkHasValidStaIp(&ip4)) {
        return WSS_GOT_IP;
      }
      // Link up but DHCP still running (SDK may report GOT_IP before netif has address).
      return WSS_CONN_SUCCESS;
    }
    return mapped;
  }

  wifi_link_status_t link = {};
  if (bk_wifi_sta_get_link_status(&link) != BK_OK) {
    return WSS_IDLE;
  }
  return wifiBkIdkMapLinkState(link.state, 0);
}

bool wifiBkIdkGetStaIp4(netif_ip4_config_t *out) {
  if (out == nullptr) {
    return false;
  }
  return bk_netif_get_ip4_config(NETIF_IF_STA, out) == BK_OK;
}

IPAddress wifiBkIdkIpFromNetif(const char *dotted) {
  if (dotted == nullptr || dotted[0] == '\0') {
    return IPAddress();
  }
  return IPAddress(dotted);
}

IPAddress wifiBkIdkIpFromU32(uint32_t addr) {
  ip4_addr_t ip4;
  ip4_addr_set_u32(&ip4, addr);
  return IPAddress(ip4_addr1(&ip4), ip4_addr2(&ip4), ip4_addr3(&ip4), ip4_addr4(&ip4));
}

bool wifiBkIdkGetApNetifIpv4(uint32_t *address, uint32_t *netmask, uint32_t *gateway) {
  wlan_ip_config cfg = {};
  net_get_if_addr(&cfg, net_get_uap_handle());
  if (address != nullptr) {
    *address = cfg.ipv4.address;
  }
  if (netmask != nullptr) {
    *netmask = cfg.ipv4.netmask;
  }
  if (gateway != nullptr) {
    *gateway = cfg.ipv4.gw;
  }
  return cfg.ipv4.address != 0;
}

bool wifiBkIdkGetStaNetifIpv4(uint32_t *address, uint32_t *netmask, uint32_t *gateway,
                              uint32_t *dns) {
  wlan_ip_config cfg = {};
  net_get_if_addr(&cfg, net_get_sta_handle());
  if (address != nullptr) {
    *address = cfg.ipv4.address;
  }
  if (netmask != nullptr) {
    *netmask = cfg.ipv4.netmask;
  }
  if (gateway != nullptr) {
    *gateway = cfg.ipv4.gw;
  }
  if (dns != nullptr) {
    *dns = cfg.ipv4.dns1;
  }
  return cfg.ipv4.address != 0;
}

WF_AP_AUTH_MODE_E wifiBkIdkMapSecurity(int security) {
  switch (static_cast<wifi_security_t>(security)) {
    case WIFI_SECURITY_NONE:
      return WAAM_OPEN;
    case WIFI_SECURITY_WEP:
      return WAAM_WEP;
    case WIFI_SECURITY_WPA_TKIP:
    case WIFI_SECURITY_WPA_AES:
    case WIFI_SECURITY_WPA_MIXED:
      return WAAM_WPA_PSK;
    case WIFI_SECURITY_WPA2_TKIP:
    case WIFI_SECURITY_WPA2_AES:
    case WIFI_SECURITY_WPA2_MIXED:
      return WAAM_WPA2_PSK;
    case WIFI_SECURITY_WPA3_SAE:
    case WIFI_SECURITY_WPA3_WPA2_MIXED:
      return WAAM_WPA_WPA3_SAE;
    default:
      return WAAM_WPA_WPA2_PSK;
  }
}

wl_status_t wifiBkIdkToWlStatus(WF_STATION_STAT_E status) {
  switch (status) {
    case WSS_CONNECTING:
      return WL_IDLE_STATUS;
    case WSS_NO_AP_FOUND:
      return WL_NO_SSID_AVAIL;
    case WSS_GOT_IP:
    case WSS_CONN_SUCCESS:
      return WL_CONNECTED;
    case WSS_CONN_FAIL:
    case WSS_PASSWD_WRONG:
    case WSS_DHCP_FAIL:
      return WL_CONNECT_FAILED;
    case WSS_IDLE:
    default:
      return WL_DISCONNECTED;
  }
}
