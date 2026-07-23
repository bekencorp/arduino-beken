#include "WiFiClient.h"
#include "WiFiGeneric.h"

extern "C" {
#include <lwip/errno.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <os/mem.h>
#include <string.h>
}

#undef connect
#undef read
#undef write
#undef close

static bool connectSocket(int fd, const struct sockaddr *addr, socklen_t addrlen, int32_t timeout_ms) {
  struct timeval tv = {};
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;
  lwip_setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  lwip_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  // Blocking connect: non-blocking + select is unreliable on this lwIP port.
  return lwip_connect(fd, addr, addrlen) == 0;
}

WiFiClient::WiFiClient() = default;

WiFiClient::WiFiClient(int fd) : _fd(fd), _connected(fd >= 0) {}

WiFiClient::~WiFiClient() {
  stop();
}

bool WiFiClient::applyTimeout(int32_t timeout_ms) {
  if (_fd < 0) {
    return false;
  }
  struct timeval tv = {};
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;
  if (lwip_setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0) {
    return false;
  }
  return lwip_setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0;
}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
  return connect(ip, port, static_cast<int32_t>(_timeout_ms));
}

int WiFiClient::connect(IPAddress ip, uint16_t port, int32_t timeout_ms) {
  stop();

  _fd = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (_fd < 0) {
    return 0;
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(port);
  const uint32_t ip_addr = static_cast<uint32_t>(ip);
  memcpy(&addr.sin_addr.s_addr, &ip_addr, sizeof(ip_addr));

  if (!connectSocket(_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), timeout_ms)) {
    stop();
    return 0;
  }

  applyTimeout(timeout_ms);
  _connected = true;
  return 1;
}

int WiFiClient::connect(const char *host, uint16_t port) {
  return connect(host, port, static_cast<int32_t>(_timeout_ms));
}

int WiFiClient::connect(const char *host, uint16_t port, int32_t timeout_ms) {
  if (host == nullptr) {
    return 0;
  }

  IPAddress addr;
  if (!WiFiGenericClass::hostByName(host, addr)) {
    return 0;
  }
  return connect(addr, port, timeout_ms);
}

size_t WiFiClient::write(uint8_t data) {
  return write(&data, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
  if (!_connected || _fd < 0 || buf == nullptr || size == 0) {
    return 0;
  }

  size_t sent_total = 0;
  int retries = 10;

  while (sent_total < size && retries > 0) {
    const int sent = lwip_send(_fd, buf + sent_total, size - sent_total, 0);
    if (sent > 0) {
      sent_total += static_cast<size_t>(sent);
      retries = 10;
      continue;
    }
    if (sent == 0) {
      break;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      retries--;
      continue;
    }
    stop();
    return sent_total;
  }

  if (sent_total == 0) {
    stop();
  }
  return sent_total;
}

int WiFiClient::available() {
  if (!_connected || _fd < 0) {
    return 0;
  }

  int pending = 0;
  if (lwip_ioctl(_fd, FIONREAD, &pending) == 0) {
    return pending;
  }

  // lwIP may not implement FIONREAD unless LWIP_SO_RCVBUF is enabled.
  uint8_t dummy = 0;
  const int peek = lwip_recv(_fd, &dummy, 1, MSG_PEEK | MSG_DONTWAIT);
  if (peek > 0) {
    return peek;
  }
  if (peek == 0) {
    stop();
  }
  return 0;
}

int WiFiClient::read() {
  uint8_t value = 0;
  if (read(&value, 1) == 1) {
    return value;
  }
  return -1;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
  if (!_connected || _fd < 0 || buf == nullptr || size == 0) {
    return 0;
  }

  const int received = lwip_recv(_fd, buf, size, 0);
  if (received < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      stop();
    }
    return 0;
  }
  if (received == 0) {
    stop();
  }
  return received;
}

int WiFiClient::peek() {
  if (!_connected || _fd < 0) {
    return -1;
  }

  uint8_t value = 0;
  const int received = lwip_recv(_fd, &value, 1, MSG_PEEK | MSG_DONTWAIT);
  if (received <= 0) {
    return -1;
  }
  return value;
}

void WiFiClient::flush() {}

void WiFiClient::stop() {
  if (_fd >= 0) {
    lwip_close(_fd);
    _fd = -1;
  }
  _connected = false;
}

uint8_t WiFiClient::connected() {
  return (_connected && _fd >= 0) ? 1u : 0u;
}

WiFiClient::operator bool() {
  return _connected && _fd >= 0;
}

int WiFiClient::fd() const {
  return _fd;
}

int WiFiClient::setTimeout(uint32_t ms) {
  _timeout_ms = ms;
  if (_fd >= 0) {
    return applyTimeout(static_cast<int32_t>(ms)) ? 0 : -1;
  }
  return 0;
}

bool WiFiClient::fillEndpoint(int fd, IPAddress &ip, uint16_t &port, bool peer) const {
  struct sockaddr_in addr = {};
  socklen_t len = sizeof(addr);
  const int rc = peer ? lwip_getpeername(fd, reinterpret_cast<struct sockaddr *>(&addr), &len)
                      : lwip_getsockname(fd, reinterpret_cast<struct sockaddr *>(&addr), &len);
  if (rc != 0) {
    return false;
  }
  ip = IPAddress(PP_NTOHL(addr.sin_addr.s_addr));
  port = PP_NTOHS(addr.sin_port);
  return true;
}

IPAddress WiFiClient::remoteIP() const {
  IPAddress ip;
  uint16_t port = 0;
  if (_fd >= 0 && fillEndpoint(_fd, ip, port, true)) {
    return ip;
  }
  return IPAddress();
}

uint16_t WiFiClient::remotePort() const {
  IPAddress ip;
  uint16_t port = 0;
  if (_fd >= 0 && fillEndpoint(_fd, ip, port, true)) {
    return port;
  }
  return 0;
}

IPAddress WiFiClient::localIP() const {
  IPAddress ip;
  uint16_t port = 0;
  if (_fd >= 0 && fillEndpoint(_fd, ip, port, false)) {
    return ip;
  }
  return IPAddress();
}

uint16_t WiFiClient::localPort() const {
  IPAddress ip;
  uint16_t port = 0;
  if (_fd >= 0 && fillEndpoint(_fd, ip, port, false)) {
    return port;
  }
  return 0;
}
