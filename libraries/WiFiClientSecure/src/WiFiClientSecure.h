#ifndef WiFiClientSecure_h
#define WiFiClientSecure_h

#include <WiFi.h>
#include "ssl_client.h"

class WiFiClientSecure : public WiFiClient
{
protected:
    sslclient_context *sslclient;

    int _lastError = 0;
    int _peek = -1;
    int _timeout = 30000;
    bool _use_insecure = false;
    const char *_CA_cert = nullptr;
    const char *_cert = nullptr;
    const char *_private_key = nullptr;
    const char *_pskIdent = nullptr; // identity for PSK cipher suites
    const char *_psKey = nullptr; // key in hex for PSK cipher suites
    const char **_alpn_protos = nullptr;
    bool _use_ca_bundle = false;

public:
    WiFiClientSecure *next = nullptr;
    WiFiClientSecure();
    explicit WiFiClientSecure(int socket);
    ~WiFiClientSecure();

    int connect(IPAddress ip, uint16_t port) override;
    int connect(IPAddress ip, uint16_t port, int32_t timeout) override;
    int connect(const char *host, uint16_t port) override;
    int connect(const char *host, uint16_t port, int32_t timeout) override;
    int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key);
    int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key);
    int connect(IPAddress ip, uint16_t port, const char *pskIdent, const char *psKey);
    int connect(const char *host, uint16_t port, const char *pskIdent, const char *psKey);
    int connect(IPAddress ip, uint16_t port, const char *host, const char *CA_cert, const char *cert, const char *private_key);

    int peek() override;
    size_t write(uint8_t data) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int available() override;
    int read() override;
    int read(uint8_t *buf, size_t size) override;
    void flush() override {}
    void stop() override;
    uint8_t connected() override;

    int lastError(char *buf, const size_t size);
    void setInsecure(); // Don't validate the chain, just accept whatever is given. VERY INSECURE!
    void setPreSharedKey(const char *pskIdent, const char *psKey); // psKey in Hex
    void setCACert(const char *rootCA);
    void setCertificate(const char *client_ca);
    void setPrivateKey(const char *private_key);
    bool loadCACert(Stream& stream, size_t size);
    void setCACertBundle(const uint8_t * bundle);
    bool loadCertificate(Stream& stream, size_t size);
    bool loadPrivateKey(Stream& stream, size_t size);
    bool verify(const char* fingerprint, const char* domain_name);
    void setHandshakeTimeout(unsigned long handshake_timeout);
    void setAlpnProtocols(const char **alpn_protos);
    const mbedtls_x509_crt* getPeerCertificate() { return mbedtls_ssl_get_peer_cert(&sslclient->ssl_ctx); };
    bool getFingerprintSHA256(uint8_t sha256_result[32]) { return get_peer_fingerprint(sslclient, sha256_result); };
    int setTimeout(uint32_t ms);
    int fd() const;

    operator bool() override
    {
        return connected();
    }
    WiFiClientSecure &operator=(const WiFiClientSecure &other);
    bool operator==(const bool value)
    {
        return bool() == value;
    }
    bool operator!=(const bool value)
    {
        return bool() != value;
    }
    bool operator==(const WiFiClientSecure &);
    bool operator!=(const WiFiClientSecure &rhs)
    {
        return !this->operator==(rhs);
    };

private:
    char *_streamLoad(Stream& stream, size_t size);
    int setSocketOption(int option, char* value, size_t len);

    using Print::write;
};

#endif /* WiFiClientSecure_h */
