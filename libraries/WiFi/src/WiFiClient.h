#ifndef WiFiClient_h
#define WiFiClient_h

#include <Arduino.h>
#include "api/Client.h"

class WiFiClient : public arduino::Client {
public:
  WiFiClient();
  explicit WiFiClient(int fd);
  virtual ~WiFiClient();

  int connect(IPAddress ip, uint16_t port) override;
  virtual int connect(IPAddress ip, uint16_t port, int32_t timeout_ms);
  int connect(const char *host, uint16_t port) override;
  virtual int connect(const char *host, uint16_t port, int32_t timeout_ms);

  size_t write(uint8_t data) override;
  size_t write(const uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  int read(uint8_t *buf, size_t size) override;
  int peek() override;
  void flush() override;
  void stop() override;
  uint8_t connected() override;
  operator bool() override;

  int fd() const;
  int setTimeout(uint32_t ms);

  IPAddress remoteIP() const;
  uint16_t remotePort() const;
  IPAddress localIP() const;
  uint16_t localPort() const;

protected:
  bool applyTimeout(int32_t timeout_ms);
  bool fillEndpoint(int fd, IPAddress &ip, uint16_t &port, bool peer) const;

  int _fd = -1;
  bool _connected = false;
  uint32_t _timeout_ms = 3000;
};

#endif
