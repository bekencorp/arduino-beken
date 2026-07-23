#include <Arduino.h>
#include "StreamString.h"

size_t StreamString::write(const uint8_t *data, size_t size) {
  if (size && data) {
    if (concat((const char *)data, size)) {
      return size;
    }
  }
  return 0;
}

size_t StreamString::write(uint8_t data) {
  return concat((char)data);
}

int StreamString::available() {
  return length();
}

int StreamString::read() {
  if (length()) {
    char c = charAt(0);
    remove(0, 1);
    return c;
  }
  return -1;
}

int StreamString::peek() {
  if (length()) {
    return charAt(0);
  }
  return -1;
}

void StreamString::flush() {
}
