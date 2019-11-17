/**
 * @brief This is a fast algorithm to caculate the digits of uint32 or uint64
 * This algorithm is based on:
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
 * benchmark:
 * https://github.com/localvoid/cxx-benchmark-count-digits
 */


#pragma once
#include <cstdint>

static const uint64_t powers_of_10_u64[] = {
  0,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000,
  10000000000,
  100000000000,
  1000000000000,
  10000000000000,
  100000000000000,
  1000000000000000,
  10000000000000000,
  100000000000000000,
  1000000000000000000,
  10000000000000000000U
};

static const uint32_t powers_of_10_u32[] = {
  0,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000
};

/// 1233>>12 means 1233/4096 ≈ 1/log2(10) = log2(n)/log10(n)
uint32_t digits10(std::uint64_t n) {
  uint64_t t = (64 - __builtin_clzll(n | 1)) * 1233 >> 12;
  return t - (n < powers_of_10_u64[t]) + 1;
}

uint32_t digits10(uint32_t n) {
  uint32_t t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
  return t - (n < powers_of_10_u32[t]) + 1;
}

uint32_t digits10(uint16_t v) {
  uint32_t n = v;
  uint32_t t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
  return t - (n < powers_of_10_u32[t]) + 1;
}

uint32_t digits10(uint8_t v) {
  uint32_t n = v;
  uint32_t t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
  return t - (n < powers_of_10_u32[t]) + 1;
}