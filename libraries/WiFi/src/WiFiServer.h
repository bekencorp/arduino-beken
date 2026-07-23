#ifndef WIFISERVER_H_
#define WIFISERVER_H_

#include <Arduino.h>
#include "api/Server.h"
#include "WiFiClient.h"

class WiFiServer : public arduino::Server {
private:
  int _sockfd = -1;
  int _accepted_sockfd = -1;
  IPAddress _addr;
  uint16_t _port;
  uint8_t _max_clients;
  bool _listening = false;
  bool _noDelay = false;

public:
  void listenOnLocalhost() {}

  WiFiServer(uint16_t port = 80, uint8_t max_clients = 4)
      : _port(port), _max_clients(max_clients) {}

  WiFiServer(const IPAddress &addr, uint16_t port = 80, uint8_t max_clients = 4)
      : _addr(addr), _port(port), _max_clients(max_clients) {}

  ~WiFiServer() { end(); }

  WiFiClient available();
  WiFiClient accept() { return available(); }
  void begin() override { begin(_port); }
  void begin(uint16_t port);
  void begin(uint16_t port, int reuse_enable);
  void setNoDelay(bool nodelay);
  bool getNoDelay();
  bool hasClient();
  size_t write(const uint8_t *data, size_t len);
  size_t write(uint8_t data) { return write(&data, 1); }
  using Print::write;

  void end();
  void close();
  void stop();
  operator bool() { return _listening; }
  int setTimeout(uint32_t seconds);
  void stopAll();
};

#endif
