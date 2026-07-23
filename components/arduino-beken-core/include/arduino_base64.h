#ifndef ARDUINO_BASE64_H_
#define ARDUINO_BASE64_H_

class base64
{
public:
    static String encode(const uint8_t * data, size_t length);
    static String encode(const String& text);
private:
};

#endif /* ARDUINO_BASE64_H_ */
