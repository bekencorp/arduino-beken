#ifndef STREAMSTRING_H_
#define STREAMSTRING_H_

#include <Arduino.h>

class StreamString : public Stream, public String {
public:
  size_t write(const uint8_t *buffer, size_t size) override;
  size_t write(uint8_t data) override;

  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
};

#endif /* STREAMSTRING_H_ */
