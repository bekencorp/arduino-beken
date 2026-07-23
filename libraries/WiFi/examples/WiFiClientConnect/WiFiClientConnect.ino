/* Wi-Fi STA Connect and Disconnect Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   Adapted for arduino-beken: uses WSS_* status codes (not ESP WL_*).
*/

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------

#include <WiFi.h>

int btnGPIO = 0;
int btnState = false;

void setup() {
  Serial.begin(115200);
  delay(500);

  // Set GPIO0 Boot button as input
  pinMode(btnGPIO, INPUT);

  // We start by connecting to a WiFi network
  // To debug, please enable Core Debug Level to Verbose

  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
// Auto reconnect is set true as default
// To set auto connect off, use the following function
//    WiFi.setAutoReconnect(false);

  // Will try for about 10 seconds (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20;

  // Wait for WiFi status (poll WSS_* until WSS_GOT_IP or failure)
  while (true) {

    switch (WiFi.status()) {
      case WSS_NO_AP_FOUND:
        Serial.println("[WiFi] SSID not found");
        break;
      case WSS_PASSWD_WRONG:
        Serial.println("[WiFi] Wrong password");
        return;
        break;
      case WSS_CONN_FAIL:
        Serial.println("[WiFi] Connection failed");
        return;
        break;
      case WSS_DHCP_FAIL:
        Serial.println("[WiFi] DHCP failed");
        return;
        break;
      case WSS_IDLE:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WSS_GOT_IP:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(static_cast<int>(WiFi.status()));
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.println("[WiFi] Failed to connect to WiFi!");
      // Use disconnect function to force stop trying to connect
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
}

void loop() {
  // Read the button state
  btnState = digitalRead(btnGPIO);

  if (btnState == LOW) {
    // Disconnect from WiFi
    Serial.println("[WiFi] Disconnecting from WiFi!");
    // This function will disconnect and turn off the WiFi (NVS WiFi data is kept)
    if (WiFi.disconnect(true, false)) {
      Serial.println("[WiFi] Disconnected from WiFi!");
    }
    delay(1000);
  }
}
