#include "WiFi.h"

WiFiClass WiFi;

void WiFiClass::printDiag(Print &dest) {
  dest.print("WiFi mode: ");
  switch (getMode()) {
    case WIFI_STA:
      dest.println("STA");
      break;
    case WIFI_AP:
      dest.println("AP");
      break;
    case WIFI_AP_STA:
      dest.println("AP+STA");
      break;
    default:
      dest.println("OFF");
      break;
  }
  dest.print("Channel: ");
  dest.println(channel());
  dest.print("Status: ");
  dest.println(static_cast<int>(status()));
}
