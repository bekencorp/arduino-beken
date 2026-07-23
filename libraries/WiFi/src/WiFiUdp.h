#ifndef WIFUDP_H_
#define WIFUDP_H_

#include <Arduino.h>
#include <Udp.h>

class WiFiUDP : public UDP {
private:
  int _udp_server = -1;
  IPAddress _multicast_ip;
  IPAddress _remote_ip;
  uint16_t _server_port = 0;
  uint16_t _remote_port = 0;
  char *_tx_buffer = nullptr;
  size_t _tx_buffer_len = 0;
  uint8_t *_rx_buffer = nullptr;
  size_t _rx_len = 0;
  size_t _rx_pos = 0;

  static constexpr size_t kUdpBufferSize = 1460;

public:
  WiFiUDP();
  ~WiFiUDP();

  uint8_t begin(IPAddress a, uint16_t p);
  uint8_t begin(uint16_t p);
  uint8_t beginMulticast(IPAddress a, uint16_t p);
  void stop();
  int beginMulticastPacket();
  int beginPacket();
  int beginPacket(IPAddress ip, uint16_t port);
  int beginPacket(const char *host, uint16_t port);
  int endPacket();
  size_t write(uint8_t) override;
  size_t write(const uint8_t *buffer, size_t size) override;
  int parsePacket();
  int available() override;
  int read() override;
  int read(unsigned char *buffer, size_t len) override;
  int read(char *buffer, size_t len) override;
  int peek() override;
  void flush() override;
  IPAddress remoteIP() override;
  uint16_t remotePort() override;
};

#endif
