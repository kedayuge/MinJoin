/**
 * implementation from https://en.wikipedia.org/wiki/MurmurHash
 *
 * modified by Nikolai Karpov <kimaska@gmail.com>
 *
 */

#ifndef MINSIMJOIN_HASH_HPP
#define MINSIMJOIN_HASH_HPP
#include <cstdlib>
#include <cstdint>

static inline uint32_t murmur_32_scramble(uint32_t k) {
  k *= 0xcc9e2d51;
  k = (k << 15) | (k >> 17);
  k *= 0x1b873593;
  return k;
}
uint32_t murmur3_32(const uint8_t *key, size_t len, uint32_t seed) {
  uint32_t h = seed;
  uint32_t k;
  for (size_t i = len >> 2; i; i--) {
    memcpy(&k, key, sizeof(uint32_t));
    key += sizeof(uint32_t);
    h ^= murmur_32_scramble(k);
    h = (h << 13) | (h >> 19);
    h = h * 5 + 0xe6546b64;
  }
  k = 0;
  for (size_t i = len & 3; i; i--) {
    k <<= 8;
    k |= key[i - 1];
  }
  h ^= murmur_32_scramble(k);
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

class SeaState {
 public:
  SeaState() :
      a(0x16f11fe89b0d677cLLU),
      b(0xb480a793d8e6c86cLLU),
      c(0x6fe2e5aaf078ebc9LLU),
      d(0x14f994a4c5259381LLU),
      length(0) {
  }
  [[nodiscard]] uint64_t finalize() const {
    return f(a ^ b ^ c ^ d ^ length);
  }
  void update(uint64_t n) {
    uint64_t new_a = f(a ^ n);
    a = b;
    b = c;
    c = d;
    d = new_a;
    length += 8;
  }

 private:
  static uint64_t f(uint64_t x) {
    static const uint64_t p = 0x6eed0e9da4d94a4fLLU;
    uint64_t f1 = x * p;
    uint64_t fa = (f1 >> 32);
    uint64_t fb = (f1 >> 60);
    uint64_t sh = fa >> fb;
    uint64_t f2 = f1 ^ sh;
    return f2 * p;
  }

  uint64_t a;
  uint64_t b;
  uint64_t c;
  uint64_t d;
  uint64_t length;
};

uint64_t seahash(const char *x, size_t len) {
  auto hash = SeaState();
  for (auto i = 0u; i < len; ++i) {
    hash.update(x[i]);
  }
  return hash.finalize();
}
#endif