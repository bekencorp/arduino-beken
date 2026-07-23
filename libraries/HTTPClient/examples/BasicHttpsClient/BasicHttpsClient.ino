/**
 * BasicHttpsClient.ino
 *
 *  Adapted for arduino-beken:
 *    - uses WiFiMulti and waits for WSS_GOT_IP before HTTPS
 *    - uses WiFiClientSecure + HTTPClient::begin(client, httpsUrl)
 *    - ships ISRG Root X1 for Let's Encrypt sites (Not After 2035-06-04)
 *
 */

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"  // Change this to your WiFi SSID
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"  // Change this to your WiFi password
#endif
// ----------------------------------------------------------

// -------- HTTPS configuration (edit before upload) --------
#ifndef HTTPS_URL
// Let's Encrypt test page signed by ISRG Root X1. Change if needed.
#define HTTPS_URL "https://valid-isrgrootx1.letsencrypt.org/"
#endif
// Set to 1 to skip certificate verification (INSECURE, debug only)
#ifndef HTTPS_INSECURE
#define HTTPS_INSECURE 0
#endif
// ----------------------------------------------------------

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

static String httpsHostFromUrl(const char *url) {
  String u(url);
  int proto = u.indexOf("://");
  if (proto < 0) {
    return u;
  }
  String rest = u.substring(proto + 3);
  int slash = rest.indexOf('/');
  if (slash >= 0) {
    rest = rest.substring(0, slash);
  }
  int colon = rest.indexOf(':');
  if (colon >= 0) {
    rest = rest.substring(0, colon);
  }
  return rest;
}

static void printTlsLastError(WiFiClientSecure &client) {
  char errBuf[160];
  const int err = client.lastError(errBuf, sizeof(errBuf));
  if (err != 0) {
    USE_SERIAL.print("[HTTPS] TLS detail (");
    USE_SERIAL.print(err);
    USE_SERIAL.print("): ");
    USE_SERIAL.println(errBuf);
  } else {
    USE_SERIAL.println("[HTTPS] TLS detail: (no mbedTLS error recorded)");
  }
}

/*
 * ISRG Root X1 (Let's Encrypt).
 * Self-signed RSA 4096 root, Not After: Jun 4 11:04:38 2035 GMT.
 * Matches HTTPS_URL above. Replace with another root CA PEM if your
 * server uses a different issuer.
 *
 * Source: https://letsencrypt.org/certs/isrgrootx1.pem
 */
const char *rootCACertificate =
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----\n";

void setup() {
  USE_SERIAL.begin(115200);
  delay(500);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.print("[SETUP] WAIT ");
    USE_SERIAL.print(t);
    USE_SERIAL.println("...");
    USE_SERIAL.flush();
    delay(1000);
  }

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
}

void loop() {
  // Wait for WiFi connection
  if (wifiMulti.run() != WL_CONNECTED) {
    USE_SERIAL.println("[HTTPS] WiFi not connected");
    delay(1000);
    return;
  }

  // Wait for WSS_GOT_IP before starting TCP/HTTPS on arduino-beken
  if (WiFi.status() != WSS_GOT_IP) {
    USE_SERIAL.println("[HTTPS] WiFi connected, waiting for IP...");
    delay(500);
    return;
  }

  // Stack-allocated secure client (avoid heap + non-virtual base destructor)
  WiFiClientSecure client;

  const String httpsHost = httpsHostFromUrl(HTTPS_URL);
  IPAddress resolvedIp;
  if (WiFi.hostByName(httpsHost.c_str(), resolvedIp)) {
    USE_SERIAL.print("[HTTPS] DNS ");
    USE_SERIAL.print(httpsHost);
    USE_SERIAL.print(" -> ");
    USE_SERIAL.println(resolvedIp);
  } else {
    USE_SERIAL.print("[HTTPS] DNS failed for ");
    USE_SERIAL.println(httpsHost);
  }

#if HTTPS_INSECURE
  // Debug only: skip certificate verification
  client.setInsecure();
  USE_SERIAL.println("[HTTPS] WARNING: certificate verification disabled");
#else
  // Trust the configured root CA for the target HTTPS server
  client.setCACert(rootCACertificate);
#endif

  {
    // Destroy HTTPClient before WiFiClientSecure goes out of scope
    HTTPClient https;
    https.setConnectTimeout(15000);
    https.setTimeout(30000);
    client.setTimeout(30000);

    USE_SERIAL.print("[HTTPS] begin...\n");
    if (https.begin(client, HTTPS_URL)) {  // HTTPS
      USE_SERIAL.print("[HTTPS] GET...\n");
      // Start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been sent and Server response header has been handled
        USE_SERIAL.print("[HTTPS] GET... code: ");
        USE_SERIAL.println(httpCode);

        // File found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          USE_SERIAL.println(payload);
        }
      } else {
        USE_SERIAL.print("[HTTPS] GET... failed, error: ");
        USE_SERIAL.println(HTTPClient::errorToString(httpCode));
        printTlsLastError(client);
      }

      https.end();
    } else {
      USE_SERIAL.println("[HTTPS] Unable to connect");
      printTlsLastError(client);
    }
  }

  USE_SERIAL.println();
  USE_SERIAL.println("Waiting 10s before the next round...");
  delay(10000);
}
