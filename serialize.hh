#pragma once

#include <cstdint>
#include <ostream>
#include <string>

/** Wrapper that formats a value as a NAR int. (64-bit LE) */
struct NarInt {
  uint64_t value;
};

inline std::ostream &operator<<(std::ostream &out, const NarInt &wrap) {
  auto n = wrap.value;
  unsigned char buf[8];
  buf[0] = n & 0xff;
  buf[1] = (n >> 8) & 0xff;
  buf[2] = (n >> 16) & 0xff;
  buf[3] = (n >> 24) & 0xff;
  buf[4] = (n >> 32) & 0xff;
  buf[5] = (n >> 40) & 0xff;
  buf[6] = (n >> 48) & 0xff;
  buf[7] = (unsigned char)(n >> 56) & 0xff;
  out.write((char *)buf, sizeof(buf));
  return out;
}

/** Wrapper that outputs zero padding to pad `len` to 8 bytes alignment. */
struct NarPad {
  size_t len;
};

inline std::ostream &operator<<(std::ostream &out, const NarPad &wrap) {
  auto rem = wrap.len % 8;
  if (rem) {
    char zero[8];
    memset(zero, 0, sizeof(zero));
    out.write(zero, 8 - rem);
  }
  return out;
}

/** Wrapper that formats a value as a NAR str. (length-prefixed) */
struct NarStr {
  std::string_view value;
};

inline std::ostream &operator<<(std::ostream &out, const NarStr &wrap) {
  auto len = wrap.value.size();
  return out << NarInt{len} << wrap.value << NarPad{len};
}
