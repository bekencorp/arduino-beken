#include "WiFiScan.h"
#include "WiFiGeneric.h"
#include "WiFiBkIdk.h"

extern "C" {
#include <common/bk_err.h>
#include <components/event.h>
#include <modules/wifi.h>
#include <modules/wifi_types.h>
#include <os/mem.h>
#include <os/os.h>
#include <os/str.h>
}

namespace {

beken_semaphore_t s_scan_done_sem = nullptr;
bool s_scan_event_registered = false;

bk_err_t scanDoneEventHandler(void *, event_module_t, int event_id, void *) {
  if (event_id == EVENT_WIFI_SCAN_DONE && s_scan_done_sem != nullptr) {
    rtos_set_semaphore(&s_scan_done_sem);
  }
  return BK_OK;
}

void ensureScanEventReady() {
  if (s_scan_done_sem == nullptr) {
    rtos_init_semaphore(&s_scan_done_sem, 1);
  }
  if (!s_scan_event_registered) {
    bk_event_register_cb(EVENT_MOD_WIFI, EVENT_WIFI_SCAN_DONE, scanDoneEventHandler, nullptr);
    s_scan_event_registered = true;
  }
}

void drainScanSemaphore() {
  if (s_scan_done_sem == nullptr) {
    return;
  }
  while (rtos_get_semaphore(&s_scan_done_sem, 0) == BK_OK) {
  }
}

bool waitScanDone(uint32_t timeout_ms) {
  ensureScanEventReady();
  return rtos_get_semaphore(&s_scan_done_sem, timeout_ms) == BK_OK;
}

}  // namespace

void WiFiScanClass::scanDelete() {
  if (_aps != nullptr) {
    os_free(_aps);
    _aps = nullptr;
  }
  _ap_num = 0;
}

const void *WiFiScanClass::_getScanInfoByIndex(uint32_t i) const {
  if (i >= _ap_num || _aps == nullptr) {
    return nullptr;
  }
  return &_aps[i];
}

int16_t WiFiScanClass::scanNetworks(bool async, bool, bool, uint32_t, uint8_t,
                                    const char *ssid, const uint8_t *bssid) {
  (void)async;
  scanDelete();
  drainScanSemaphore();

  wifi_scan_config_t scan_config = {};
  const wifi_scan_config_t *scan_cfg = nullptr;
  if (ssid != nullptr) {
    os_strlcpy(scan_config.ssid, ssid, sizeof(scan_config.ssid));
    scan_cfg = &scan_config;
  }
  if (bssid != nullptr) {
    os_memcpy(scan_config.bssid, bssid, WIFI_BSSID_LEN);
    scan_cfg = &scan_config;
  }

  if (bk_wifi_scan_start(scan_cfg) != BK_OK) {
    return WIFI_SCAN_FAILED;
  }

  constexpr uint32_t kScanTimeoutMs = 20000;
  if (!waitScanDone(kScanTimeoutMs)) {
    bk_wifi_scan_stop();
    return WIFI_SCAN_FAILED;
  }

  wifi_scan_result_t result = {};
  if (bk_wifi_scan_get_result(&result) != BK_OK) {
    bk_wifi_scan_free_result(&result);
    return WIFI_SCAN_FAILED;
  }

  if (result.ap_num <= 0) {
    bk_wifi_scan_free_result(&result);
    return 0;
  }

  _ap_num = static_cast<uint32_t>(result.ap_num);
  _aps = static_cast<ScanApEntry *>(os_malloc(_ap_num * sizeof(ScanApEntry)));
  if (_aps == nullptr) {
    bk_wifi_scan_free_result(&result);
    _ap_num = 0;
    return WIFI_SCAN_FAILED;
  }

  for (int i = 0; i < result.ap_num; i++) {
    os_memset(&_aps[i], 0, sizeof(ScanApEntry));
    os_strlcpy(_aps[i].ssid, result.aps[i].ssid, sizeof(_aps[i].ssid));
    os_memcpy(_aps[i].bssid, result.aps[i].bssid, sizeof(_aps[i].bssid));
    _aps[i].rssi = result.aps[i].rssi;
    _aps[i].channel = result.aps[i].channel;
    _aps[i].security = static_cast<uint8_t>(result.aps[i].security);
  }

  bk_wifi_scan_free_result(&result);
  return static_cast<int16_t>(_ap_num);
}

bool WiFiScanClass::getNetworkInfo(uint32_t networkItem, String &ssid, uint8_t &encryptionType,
                                 int32_t &rssi, uint8_t *&bssidOut, int32_t &channelOut) {
  const ScanApEntry *it = static_cast<const ScanApEntry *>(_getScanInfoByIndex(networkItem));
  if (it == nullptr) {
    return false;
  }
  ssid = it->ssid;
  encryptionType = static_cast<uint8_t>(wifiBkIdkMapSecurity(it->security));
  rssi = it->rssi;
  bssidOut = const_cast<uint8_t *>(it->bssid);
  channelOut = it->channel;
  return true;
}

String WiFiScanClass::SSID(uint8_t networkItem) {
  const ScanApEntry *it = static_cast<const ScanApEntry *>(_getScanInfoByIndex(networkItem));
  if (it == nullptr) {
    return String();
  }
  return String(it->ssid);
}

WF_AP_AUTH_MODE_E WiFiScanClass::encryptionType(uint8_t networkItem) {
  const ScanApEntry *it = static_cast<const ScanApEntry *>(_getScanInfoByIndex(networkItem));
  if (it == nullptr) {
    return WAAM_OPEN;
  }
  return wifiBkIdkMapSecurity(it->security);
}

int32_t WiFiScanClass::RSSI(uint8_t networkItem) {
  const ScanApEntry *it = static_cast<const ScanApEntry *>(_getScanInfoByIndex(networkItem));
  if (it == nullptr) {
    return 0;
  }
  return it->rssi;
}

uint8_t *WiFiScanClass::BSSID(uint8_t networkItem) {
  ScanApEntry *it = const_cast<ScanApEntry *>(
      static_cast<const ScanApEntry *>(_getScanInfoByIndex(networkItem)));
  if (it == nullptr) {
    return nullptr;
  }
  return it->bssid;
}

String WiFiScanClass::BSSIDstr(uint8_t networkItem) {
  uint8_t *bssid = BSSID(networkItem);
  if (bssid == nullptr) {
    return String();
  }
  char bssidStr[18] = {};
  snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1],
           bssid[2], bssid[3], bssid[4], bssid[5]);
  return String(bssidStr);
}

int32_t WiFiScanClass::channel(uint8_t networkItem) {
  const ScanApEntry *it = static_cast<const ScanApEntry *>(_getScanInfoByIndex(networkItem));
  if (it == nullptr) {
    return 0;
  }
  return it->channel;
}
