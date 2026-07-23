#include "WiFiUdp.h"
#include "WiFiGeneric.h"

extern "C" {
#include <lwip/errno.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <os/mem.h>
#include <string.h>
}

#undef write
#undef read
#undef close

WiFiUDP::WiFiUDP() = default;

WiFiUDP::~WiFiUDP() {
  stop();
}

void WiFiUDP::stop() {
  if (_tx_buffer != nullptr) {
    os_free(_tx_buffer);
    _tx_buffer = nullptr;
  }
  _tx_buffer_len = 0;

  if (_rx_buffer != nullptr) {
    os_free(_rx_buffer);
    _rx_buffer = nullptr;
  }
  _rx_len = 0;
  _rx_pos = 0;

  if (_udp_server < 0) {
    return;
  }

  if (_multicast_ip != IPAddress((uint32_t)INADDR_ANY)) {
    struct ip_mreq mreq = {};
    mreq.imr_multiaddr.s_addr = static_cast<uint32_t>(_multicast_ip);
    mreq.imr_interface.s_addr = INADDR_ANY;
    lwip_setsockopt(_udp_server, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    _multicast_ip = IPAddress((uint32_t)INADDR_ANY);
  }

  lwip_close(_udp_server);
  _udp_server = -1;
  _server_port = 0;
}

uint8_t WiFiUDP::begin(IPAddress address, uint16_t port) {
  stop();
  _server_port = port;

  _tx_buffer = static_cast<char *>(os_malloc(kUdpBufferSize));
  if (_tx_buffer == nullptr) {
    return 0;
  }

  _udp_server = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  if (_udp_server < 0) {
    stop();
    return 0;
  }

  int yes = 1;
  if (lwip_setsockopt(_udp_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
    stop();
    return 0;
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(port);
  addr.sin_addr.s_addr = static_cast<uint32_t>(address);

  if (lwip_bind(_udp_server, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    stop();
    return 0;
  }

  int nonblock = 1;
  lwip_ioctl(_udp_server, FIONBIO, &nonblock);
  return 1;
}

uint8_t WiFiUDP::begin(uint16_t p) {
  return begin(IPAddress((uint32_t)INADDR_ANY), p);
}

uint8_t WiFiUDP::beginMulticast(IPAddress a, uint16_t p) {
  if (!begin(IPAddress((uint32_t)INADDR_ANY), p)) {
    return 0;
  }
  if (a != IPAddress((uint32_t)0)) {
    struct ip_mreq mreq = {};
    mreq.imr_multiaddr.s_addr = static_cast<uint32_t>(a);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (lwip_setsockopt(_udp_server, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
      stop();
      return 0;
    }
    _multicast_ip = a;
  }
  return 1;
}

int WiFiUDP::beginMulticastPacket() {
  if (!_server_port || _multicast_ip == IPAddress((uint32_t)INADDR_ANY)) {
    return 0;
  }
  _remote_ip = _multicast_ip;
  _remote_port = _server_port;
  return beginPacket();
}

int WiFiUDP::beginPacket() {
  if (!_remote_port) {
    return 0;
  }

  if (_tx_buffer == nullptr) {
    _tx_buffer = static_cast<char *>(os_malloc(kUdpBufferSize));
    if (_tx_buffer == nullptr) {
      return 0;
    }
  }
  _tx_buffer_len = 0;

  if (_udp_server >= 0) {
    return 1;
  }

  _udp_server = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  if (_udp_server < 0) {
    return 0;
  }
  int nonblock = 1;
  lwip_ioctl(_udp_server, FIONBIO, &nonblock);
  return 1;
}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port) {
  _remote_ip = ip;
  _remote_port = port;
  return beginPacket();
}

int WiFiUDP::beginPacket(const char *host, uint16_t port) {
  IPAddress addr;
  if (!WiFiGenericClass::hostByName(host, addr)) {
    return 0;
  }
  return beginPacket(addr, port);
}

int WiFiUDP::endPacket() {
  struct sockaddr_in recipient = {};
  recipient.sin_addr.s_addr = static_cast<uint32_t>(_remote_ip);
  recipient.sin_family = AF_INET;
  recipient.sin_port = PP_HTONS(_remote_port);

  const int sent = lwip_sendto(_udp_server, _tx_buffer, _tx_buffer_len, 0,
                               reinterpret_cast<struct sockaddr *>(&recipient), sizeof(recipient));
  if (sent < 0) {
    return 0;
  }
  return 1;
}

size_t WiFiUDP::write(uint8_t data) {
  if (_tx_buffer_len == kUdpBufferSize) {
    endPacket();
    _tx_buffer_len = 0;
  }
  _tx_buffer[_tx_buffer_len++] = static_cast<char>(data);
  return 1;
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size) {
  if (buffer == nullptr) {
    return 0;
  }
  for (size_t i = 0; i < size; i++) {
    write(buffer[i]);
  }
  return size;
}

int WiFiUDP::parsePacket() {
  if (_rx_buffer != nullptr) {
    return 0;
  }

  struct sockaddr_in peer = {};
  socklen_t slen = sizeof(peer);
  uint8_t *buf = static_cast<uint8_t *>(os_malloc(kUdpBufferSize));
  if (buf == nullptr) {
    return 0;
  }

  const int len = lwip_recvfrom(_udp_server, buf, kUdpBufferSize, MSG_DONTWAIT,
                                reinterpret_cast<struct sockaddr *>(&peer), &slen);
  if (len < 0) {
    os_free(buf);
    if (errno == EWOULDBLOCK) {
      return 0;
    }
    return 0;
  }

  // Keep sin_addr.s_addr byte order (same as BK7238 / endPacket cast). Do not PP_NTOHL.
  _remote_ip = IPAddress(static_cast<uint32_t>(peer.sin_addr.s_addr));
  _remote_port = PP_NTOHS(peer.sin_port);
  _rx_buffer = buf;
  _rx_len = static_cast<size_t>(len);
  _rx_pos = 0;
  return len;
}

int WiFiUDP::available() {
  if (_rx_buffer == nullptr) {
    return 0;
  }
  return static_cast<int>(_rx_len - _rx_pos);
}

int WiFiUDP::read() {
  if (_rx_buffer == nullptr || _rx_pos >= _rx_len) {
    return -1;
  }
  return _rx_buffer[_rx_pos++];
}

int WiFiUDP::read(unsigned char *buffer, size_t len) {
  return read(reinterpret_cast<char *>(buffer), len);
}

int WiFiUDP::read(char *buffer, size_t len) {
  if (_rx_buffer == nullptr || buffer == nullptr || len == 0) {
    return 0;
  }
  const size_t to_read = (_rx_len - _rx_pos) < len ? (_rx_len - _rx_pos) : len;
  memcpy(buffer, _rx_buffer + _rx_pos, to_read);
  _rx_pos += to_read;
  if (_rx_pos >= _rx_len) {
    os_free(_rx_buffer);
    _rx_buffer = nullptr;
    _rx_len = 0;
    _rx_pos = 0;
  }
  return static_cast<int>(to_read);
}

int WiFiUDP::peek() {
  if (_rx_buffer == nullptr || _rx_pos >= _rx_len) {
    return -1;
  }
  return _rx_buffer[_rx_pos];
}

void WiFiUDP::flush() {
  if (_rx_buffer != nullptr) {
    os_free(_rx_buffer);
    _rx_buffer = nullptr;
  }
  _rx_len = 0;
  _rx_pos = 0;
}

IPAddress WiFiUDP::remoteIP() {
  return _remote_ip;
}

uint16_t WiFiUDP::remotePort() {
  return _remote_port;
}
