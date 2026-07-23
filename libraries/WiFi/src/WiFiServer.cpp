#include "WiFiServer.h"

extern "C" {
#include <lwip/errno.h>
#include <lwip/sockets.h>
}

#undef write
#undef close

int WiFiServer::setTimeout(uint32_t seconds) {
  struct timeval tv = {};
  tv.tv_sec = seconds;
  tv.tv_usec = 0;
  if (lwip_setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    return -1;
  }
  return lwip_setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

size_t WiFiServer::write(const uint8_t *, size_t) {
  return 0;
}

void WiFiServer::stopAll() {}

WiFiClient WiFiServer::available() {
  if (!_listening) {
    return WiFiClient();
  }

  int client_sock;
  if (_accepted_sockfd >= 0) {
    client_sock = _accepted_sockfd;
    _accepted_sockfd = -1;
  } else {
    struct sockaddr_in client = {};
    socklen_t cs = sizeof(client);
    client_sock = lwip_accept(_sockfd, reinterpret_cast<struct sockaddr *>(&client), &cs);
  }

  if (client_sock >= 0) {
    int val = 1;
    if (lwip_setsockopt(client_sock, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == 0) {
      val = _noDelay ? 1 : 0;
      if (lwip_setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == 0) {
        return WiFiClient(client_sock);
      }
    }
    lwip_close(client_sock);
  }
  return WiFiClient();
}

void WiFiServer::begin(uint16_t port) {
  begin(port, 1);
}

void WiFiServer::begin(uint16_t port, int enable) {
  if (_listening) {
    return;
  }
  if (port) {
    _port = port;
  }

  struct sockaddr_in server = {};
  _sockfd = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    return;
  }

  lwip_setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = static_cast<uint32_t>(_addr);
  server.sin_port = PP_HTONS(_port);

  if (lwip_bind(_sockfd, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0) {
    end();
    return;
  }
  if (lwip_listen(_sockfd, _max_clients) < 0) {
    end();
    return;
  }

  int nonblock = 1;
  lwip_ioctl(_sockfd, FIONBIO, &nonblock);
  _listening = true;
  _noDelay = false;
  _accepted_sockfd = -1;
}

void WiFiServer::setNoDelay(bool nodelay) {
  _noDelay = nodelay;
}

bool WiFiServer::getNoDelay() {
  return _noDelay;
}

bool WiFiServer::hasClient() {
  if (_accepted_sockfd >= 0) {
    return true;
  }

  struct sockaddr_in client = {};
  socklen_t cs = sizeof(client);
  _accepted_sockfd =
      lwip_accept(_sockfd, reinterpret_cast<struct sockaddr *>(&client), &cs);
  return _accepted_sockfd >= 0;
}

void WiFiServer::end() {
  if (_sockfd >= 0) {
    lwip_close(_sockfd);
    _sockfd = -1;
  }
  _accepted_sockfd = -1;
  _listening = false;
}

void WiFiServer::close() {
  end();
}

void WiFiServer::stop() {
  end();
}
