#include "DNSServer.h"
#include <Arduino.h>
#include <lwip/def.h>

// SoftAP DHCP always calls dhcp_enable_nack_dns_server(), which binds UDP/53 and
// answers every query with REFUSED. That blocks Arduino DNSServer captive-portal
// hijacking. Provide a strong empty override so DNSServer can own port 53.
extern "C" void dhcp_enable_nack_dns_server(void) {}

// #define DEBUG_DNS
#ifdef DEBUG_DNS
#define DEBUG_OUTPUT Serial
#else
#define DEBUG_OUTPUT Serial
#endif

DNSServer::DNSServer() {
  _ttl = htonl(DNS_DEFAULT_TTL);
  _errorReplyCode = DNSReplyCode::NonExistentDomain;
  _dnsHeader = (DNSHeader *)malloc(sizeof(DNSHeader));
  _dnsQuestion = (DNSQuestion *)malloc(sizeof(DNSQuestion));
  _buffer = NULL;
  _currentPacketSize = 0;
  _port = 0;
}

DNSServer::~DNSServer() {
  if (_dnsHeader) {
    free(_dnsHeader);
    _dnsHeader = NULL;
  }
  if (_dnsQuestion) {
    free(_dnsQuestion);
    _dnsQuestion = NULL;
  }
  if (_buffer) {
    free(_buffer);
    _buffer = NULL;
  }
}

bool DNSServer::start(const uint16_t &port, const String &domainName, const IPAddress &resolvedIP) {
  _port = port;
  _buffer = NULL;
  _domainName = domainName;
  _resolvedIP[0] = resolvedIP[0];
  _resolvedIP[1] = resolvedIP[1];
  _resolvedIP[2] = resolvedIP[2];
  _resolvedIP[3] = resolvedIP[3];
  downcaseAndRemoveWwwPrefix(_domainName);
  return _udp.begin(_port) == 1;
}

void DNSServer::setErrorReplyCode(const DNSReplyCode &replyCode) {
  _errorReplyCode = replyCode;
}

void DNSServer::setTTL(const uint32_t &ttl) {
  _ttl = htonl(ttl);
}

void DNSServer::stop() {
  _udp.stop();
  free(_buffer);
  _buffer = NULL;
}

void DNSServer::downcaseAndRemoveWwwPrefix(String &domainName) {
  domainName.toLowerCase();
  domainName.replace("www.", "");
}

void DNSServer::processNextRequest() {
  if (_dnsHeader == NULL || _dnsQuestion == NULL) {
    return;
  }

  _currentPacketSize = _udp.parsePacket();
  if (_currentPacketSize) {
    if (_buffer != NULL) {
      free(_buffer);
    }
    _buffer = (unsigned char *)malloc(_currentPacketSize * sizeof(char));
    if (_buffer == NULL) {
      return;
    }

    _udp.read(_buffer, _currentPacketSize);
    memcpy(_dnsHeader, _buffer, DNS_HEADER_SIZE);
    if (requestIncludesOnlyOneQuestion()) {
      // QName is a sequence of length-prefixed labels, terminated by 0
      _dnsQuestion->QNameLength = 0;
      while (_buffer[DNS_HEADER_SIZE + _dnsQuestion->QNameLength] != 0) {
        memcpy((void *)&_dnsQuestion->QName[_dnsQuestion->QNameLength],
               (void *)&_buffer[DNS_HEADER_SIZE + _dnsQuestion->QNameLength],
               _buffer[DNS_HEADER_SIZE + _dnsQuestion->QNameLength] + 1);
        _dnsQuestion->QNameLength +=
            _buffer[DNS_HEADER_SIZE + _dnsQuestion->QNameLength] + 1;
      }
      _dnsQuestion->QName[_dnsQuestion->QNameLength] = 0;
      _dnsQuestion->QNameLength++;

      memcpy(&_dnsQuestion->QType,
             (void *)&_buffer[DNS_HEADER_SIZE + _dnsQuestion->QNameLength],
             sizeof(_dnsQuestion->QType));
      memcpy(&_dnsQuestion->QClass,
             (void *)&_buffer[DNS_HEADER_SIZE + _dnsQuestion->QNameLength +
                              sizeof(_dnsQuestion->QType)],
             sizeof(_dnsQuestion->QClass));
    }

    if (_dnsHeader->QR == DNS_QR_QUERY && _dnsHeader->OPCode == DNS_OPCODE_QUERY &&
        requestIncludesOnlyOneQuestion() &&
        (_domainName == "*" || getDomainNameWithoutWwwPrefix() == _domainName)) {
      replyWithIP();
    } else if (_dnsHeader->QR == DNS_QR_QUERY) {
      replyWithCustomCode();
    }

    free(_buffer);
    _buffer = NULL;
  }
}

bool DNSServer::requestIncludesOnlyOneQuestion() {
  return ntohs(_dnsHeader->QDCount) == 1 && _dnsHeader->ANCount == 0 &&
         _dnsHeader->NSCount == 0 && _dnsHeader->ARCount == 0;
}

String DNSServer::getDomainNameWithoutWwwPrefix() {
  String parsedDomainName = "";
  if (_buffer == NULL) {
    return parsedDomainName;
  }

  unsigned char *start = _buffer + DNS_OFFSET_DOMAIN_NAME;
  if (*start == 0) {
    return parsedDomainName;
  }

  int pos = 0;
  while (true) {
    unsigned char labelLength = *(start + pos);
    for (int i = 0; i < labelLength; i++) {
      pos++;
      parsedDomainName += (char)*(start + pos);
    }
    pos++;
    if (*(start + pos) == 0) {
      downcaseAndRemoveWwwPrefix(parsedDomainName);
      return parsedDomainName;
    } else {
      parsedDomainName += ".";
    }
  }
}

void DNSServer::replyWithIP() {
  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());

  _dnsHeader->QR = DNS_QR_RESPONSE;
  _dnsHeader->ANCount = _dnsHeader->QDCount;
  _udp.write((unsigned char *)_dnsHeader, DNS_HEADER_SIZE);

  _udp.write(_dnsQuestion->QName, _dnsQuestion->QNameLength);
  _udp.write((unsigned char *)&_dnsQuestion->QType, 2);
  _udp.write((unsigned char *)&_dnsQuestion->QClass, 2);

  // DNS name compression pointer to offset of QName in the message
  _udp.write((uint8_t)0xC0);
  _udp.write((uint8_t)DNS_OFFSET_DOMAIN_NAME);

  uint16_t answerType = htons(DNS_TYPE_A);
  uint16_t answerClass = htons(DNS_CLASS_IN);
  uint16_t answerIPv4 = htons(DNS_RDLENGTH_IPV4);
  _udp.write((unsigned char *)&answerType, 2);
  _udp.write((unsigned char *)&answerClass, 2);
  _udp.write((unsigned char *)&_ttl, 4);
  _udp.write((unsigned char *)&answerIPv4, 2);
  _udp.write(_resolvedIP, sizeof(_resolvedIP));
  _udp.endPacket();

#ifdef DEBUG_DNS
  DEBUG_OUTPUT.printf("DNS responds: %s for %s\n",
                      IPAddress(_resolvedIP).toString().c_str(),
                      getDomainNameWithoutWwwPrefix().c_str());
#endif
}

void DNSServer::replyWithCustomCode() {
  _dnsHeader->QR = DNS_QR_RESPONSE;
  _dnsHeader->RCode = (unsigned char)_errorReplyCode;
  _dnsHeader->QDCount = 0;

  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write((unsigned char *)_dnsHeader, sizeof(DNSHeader));
  _udp.endPacket();
}
