#ifndef WIFI_BK_IDK_H_
#define WIFI_BK_IDK_H_

#include <Arduino.h>
#include "WiFiType.h"

extern "C" {
#include <components/netif_types.h>
}

WF_STATION_STAT_E wifiBkIdkPollStaStatus();
WF_STATION_STAT_E wifiBkIdkMapLinkState(int link_state, int reason_code);
bool wifiBkIdkGetStaIp4(netif_ip4_config_t *out);
IPAddress wifiBkIdkIpFromNetif(const char *dotted);
IPAddress wifiBkIdkIpFromU32(uint32_t addr);
bool wifiBkIdkGetApNetifIpv4(uint32_t *address, uint32_t *netmask, uint32_t *gateway);
bool wifiBkIdkGetStaNetifIpv4(uint32_t *address, uint32_t *netmask, uint32_t *gateway,
                              uint32_t *dns);
WF_AP_AUTH_MODE_E wifiBkIdkMapSecurity(int security);
wl_status_t wifiBkIdkToWlStatus(WF_STATION_STAT_E status);

#endif
