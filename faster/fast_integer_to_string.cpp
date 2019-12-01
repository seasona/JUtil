/// Andrei alexandrescu three optimization tips for c++
/// 提到一个格式化数字为字符串的优化因为整数除法很慢，所以每次除以两位，并到映射中
/// 查找来节省除法的时间，可以看到每次除以两位的速度几乎是普通每次除一位方法的两倍，
/// 更是to_string的5倍以上，这是一种比较好的优化方法
/// google开源的abseil库中的FastIntToBuffer也使用了该思想，当然abseil库中还对分
/// 支跳转进行了优化，大的数放在前面以期望编译器产生没有分支跳转的块

#include <benchmark/benchmark.h>
#include <stdint.h>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>

const uint64_t num = 922372036854775821;
// const uint32_t num = 82635722;
enum { buffer_size = std::numeric_limits<long long>::digits10 + 5 };
char str[buffer_size];

template <typename T>
uint32_t uint64ToAscii(T v, char* const buffer) {
    auto const result = std::numeric_limits<T>::digits10;
    uint32_t pos = result - 1;
    while (v >= 10) {
        auto const q = v / 10;
        auto const r = static_cast<uint32_t>(v % 10);
        buffer[pos--] = '0' + r;
        v = q;
    }
    // assert(pos == 0); // Last digit is trivial to handle
    *buffer = static_cast<uint32_t>(v) + '0';
    return result;
}

static const char digits[201] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

template <typename T>
unsigned u64ToAsciiTable(T value, char* dst) {
    uint32_t const length = std::numeric_limits<T>::digits10;
    uint32_t next = length - 1;
    while (value >= 100) {
        auto const i = (value % 100) * 2;
        value /= 100;
        dst[next] = digits[i + 1];
        dst[next - 1] = digits[i];
        next -= 2;
    }
    // Handle last 1-2 digits
    if (value < 10) {
        dst[next] = '0' + uint32_t(value);
    } else {
        auto i = uint32_t(value) * 2;
        dst[next] = digits[i + 1];
        dst[next - 1] = digits[i];
    }
    return length;
}

//================fast int to string in google's base library
//<abseil>===========
const char one_ASCII_final_digits[10][2]{
    {'0', 0}, {'1', 0}, {'2', 0}, {'3', 0}, {'4', 0},
    {'5', 0}, {'6', 0}, {'7', 0}, {'8', 0}, {'9', 0},
};

const char two_ASCII_digits[100][2] = {
    {'0', '0'}, {'0', '1'}, {'0', '2'}, {'0', '3'}, {'0', '4'}, {'0', '5'},
    {'0', '6'}, {'0', '7'}, {'0', '8'}, {'0', '9'}, {'1', '0'}, {'1', '1'},
    {'1', '2'}, {'1', '3'}, {'1', '4'}, {'1', '5'}, {'1', '6'}, {'1', '7'},
    {'1', '8'}, {'1', '9'}, {'2', '0'}, {'2', '1'}, {'2', '2'}, {'2', '3'},
    {'2', '4'}, {'2', '5'}, {'2', '6'}, {'2', '7'}, {'2', '8'}, {'2', '9'},
    {'3', '0'}, {'3', '1'}, {'3', '2'}, {'3', '3'}, {'3', '4'}, {'3', '5'},
    {'3', '6'}, {'3', '7'}, {'3', '8'}, {'3', '9'}, {'4', '0'}, {'4', '1'},
    {'4', '2'}, {'4', '3'}, {'4', '4'}, {'4', '5'}, {'4', '6'}, {'4', '7'},
    {'4', '8'}, {'4', '9'}, {'5', '0'}, {'5', '1'}, {'5', '2'}, {'5', '3'},
    {'5', '4'}, {'5', '5'}, {'5', '6'}, {'5', '7'}, {'5', '8'}, {'5', '9'},
    {'6', '0'}, {'6', '1'}, {'6', '2'}, {'6', '3'}, {'6', '4'}, {'6', '5'},
    {'6', '6'}, {'6', '7'}, {'6', '8'}, {'6', '9'}, {'7', '0'}, {'7', '1'},
    {'7', '2'}, {'7', '3'}, {'7', '4'}, {'7', '5'}, {'7', '6'}, {'7', '7'},
    {'7', '8'}, {'7', '9'}, {'8', '0'}, {'8', '1'}, {'8', '2'}, {'8', '3'},
    {'8', '4'}, {'8', '5'}, {'8', '6'}, {'8', '7'}, {'8', '8'}, {'8', '9'},
    {'9', '0'}, {'9', '1'}, {'9', '2'}, {'9', '3'}, {'9', '4'}, {'9', '5'},
    {'9', '6'}, {'9', '7'}, {'9', '8'}, {'9', '9'}};

inline void PutTwoDigits(size_t i, char* buf) {
    assert(i < 100);
    memcpy(buf, two_ASCII_digits[i], 2);
}

char* FastIntToBuffer(uint32_t i, char* buffer) {
    uint32_t digits;
    // The idea of this implementation is to trim the number of divides to as
    // few as possible, and also reducing memory stores and branches, by going
    // in steps of two digits at a time rather than one whenever possible. The
    // huge-number case is first, in the hopes that the compiler will output
    // that case in one branch-free block of code, and only output conditional
    // branches into it from below.
    if (i >= 1000000000) {       // >= 1,000,000,000
        digits = i / 100000000;  //      100,000,000
        i -= digits * 100000000;
        PutTwoDigits(digits, buffer);
        buffer += 2;
    lt100_000_000:
        digits = i / 1000000;  // 1,000,000
        i -= digits * 1000000;
        PutTwoDigits(digits, buffer);
        buffer += 2;
    lt1_000_000:
        digits = i / 10000;  // 10,000
        i -= digits * 10000;
        PutTwoDigits(digits, buffer);
        buffer += 2;
    lt10_000:
        digits = i / 100;
        i -= digits * 100;
        PutTwoDigits(digits, buffer);
        buffer += 2;
    lt100:
        digits = i;
        PutTwoDigits(digits, buffer);
        buffer += 2;
        *buffer = 0;
        return buffer;
    }

    if (i < 100) {
        digits = i;
        if (i >= 10) goto lt100;
        memcpy(buffer, one_ASCII_final_digits[i], 2);
        return buffer + 1;
    }
    if (i < 10000) {  //    10,000
        if (i >= 1000) goto lt10_000;
        digits = i / 100;
        i -= digits * 100;
        *buffer++ = '0' + digits;
        goto lt100;
    }
    if (i < 1000000) {  //    1,000,000
        if (i >= 100000) goto lt1_000_000;
        digits = i / 10000;  //    10,000
        i -= digits * 10000;
        *buffer++ = '0' + digits;
        goto lt10_000;
    }
    if (i < 100000000) {  //    100,000,000
        if (i >= 10000000) goto lt100_000_000;
        digits = i / 1000000;  //   1,000,000
        i -= digits * 1000000;
        *buffer++ = '0' + digits;
        goto lt1_000_000;
    }
    // we already know that i < 1,000,000,000
    digits = i / 100000000;  //   100,000,000
    i -= digits * 100000000;
    *buffer++ = '0' + digits;
    goto lt100_000_000;
}

char* FastIntToBuffer(uint64_t i, char* buffer) {
    uint32_t u32 = static_cast<uint32_t>(i);
    if (u32 == i) return FastIntToBuffer(u32, buffer);

    // Here we know i has at least 10 decimal digits.
    uint64_t top_1to11 = i / 1000000000;
    u32 = static_cast<uint32_t>(i - top_1to11 * 1000000000);
    uint32_t top_1to11_32 = static_cast<uint32_t>(top_1to11);

    if (top_1to11_32 == top_1to11) {
        buffer = FastIntToBuffer(top_1to11_32, buffer);
    } else {
        // top_1to11 has more than 32 bits too; print it in two steps.
        uint32_t top_8to9 = static_cast<uint32_t>(top_1to11 / 100);
        uint32_t mid_2 = static_cast<uint32_t>(top_1to11 - top_8to9 * 100);
        buffer = FastIntToBuffer(top_8to9, buffer);
        PutTwoDigits(mid_2, buffer);
        buffer += 2;
    }

    // We have only 9 digits now, again the maximum uint32_t can handle fully.
    uint32_t digits = u32 / 10000000;  // 10,000,000
    u32 -= digits * 10000000;
    PutTwoDigits(digits, buffer);
    buffer += 2;
    digits = u32 / 100000;  // 100,000
    u32 -= digits * 100000;
    PutTwoDigits(digits, buffer);
    buffer += 2;
    digits = u32 / 1000;  // 1,000
    u32 -= digits * 1000;
    PutTwoDigits(digits, buffer);
    buffer += 2;
    digits = u32 / 10;
    u32 -= digits * 10;
    PutTwoDigits(digits, buffer);
    buffer += 2;
    memcpy(buffer, one_ASCII_final_digits[u32], 2);
    return buffer + 1;
}

// from CO library: https://github.com/idealvin/co/blob/master/base/fast.cc
// it's a little faster than abseil in O2 but much slower in O3
static uint16_t itoh_table[256];
static uint32_t itoa_table[10000];

int init_table() {
    for (int i = 0; i < 256; ++i) {
        char* b = (char*)(itoh_table + i);
        b[0] = "0123456789abcdef"[i >> 4];
        b[1] = "0123456789abcdef"[i & 0x0f];
    }

    for (int i = 0; i < 10000; ++i) {
        char* b = (char*)(itoa_table + i);
        b[3] = (char)(i % 10 + '0');
        b[2] = (char)(i % 100 / 10 + '0');
        b[1] = (char)(i % 1000 / 100 + '0');
        b[0] = (char)(i / 1000);

        // digits of i: (b[0] >> 4) + 1
        if (i > 999) {
            b[0] |= (3 << 4);  // 0x30
        } else if (i > 99) {
            b[0] |= (2 << 4);  // 0x20
        } else if (i > 9) {
            b[0] |= (1 << 4);  // 0x10
        }
    }

    return 0;
}

static int _initialized = init_table();

int COu64toa(uint64_t v, char* buf) {
    uint32_t b[5], *p = b + 4;
    uint64_t x;

    if (v > 9999) {
        x = v / 10000;
        b[4] = itoa_table[v - x * 10000] | 0x30303030;
        --p;
    } else {
        b[4] = itoa_table[v];
        goto u64toa_end;
    }

    if (x > 9999) {
        v = x / 10000;
        b[3] = itoa_table[x - v * 10000] | 0x30303030;
        --p;
    } else {
        b[3] = itoa_table[x];
        goto u64toa_end;
    }

    if (v > 9999) {
        x = v / 10000;
        b[2] = itoa_table[v - x * 10000] | 0x30303030;
        --p;
    } else {
        b[2] = itoa_table[v];
        goto u64toa_end;
    }

    if (x > 9999) {
        b[0] = (uint32_t)(x / 10000);
        b[1] = itoa_table[x - b[0] * 10000] | 0x30303030;
        b[0] = itoa_table[b[0]];
        --p;
    } else {
        b[1] = itoa_table[x];
        goto u64toa_end;
    }

u64toa_end:
    int len = (int)(((char*)b) + 17 + ((*(char*)p) >> 4) - ((char*)p));
    memcpy(buf, ((char*)b) + 20 - len, (size_t)len);
    return len;
}

static void BM_uint64ToAscii(benchmark::State& state) {
    for (auto _ : state) {
        uint64ToAscii(num, str);
    }
}

static void BM_u64ToAsciiTable(benchmark::State& state) {
    for (auto _ : state) {
        u64ToAsciiTable(num, str);
    }
}

static void BM_toString(benchmark::State& state) {
    for (auto _ : state) {
        std::to_string(num);
    }
}

static void BM_abseilToString(benchmark::State& state) {
    for (auto _ : state) {
        FastIntToBuffer(num, str);
    }
}

static void BM_COu64toa(benchmark::State& state) {
    for (auto _ : state) {
        COu64toa(num, str);
    }
}

BENCHMARK(BM_uint64ToAscii);
BENCHMARK(BM_u64ToAsciiTable);
BENCHMARK(BM_toString);
BENCHMARK(BM_abseilToString);
BENCHMARK(BM_COu64toa);
BENCHMARK_MAIN();

// benchmark condition:
// g++ 7.4.0 
// const int num = 82635722
// -------------------------------------------------------------
// Benchmark                   Time             CPU   Iterations
// -------------------------------------------------------------
// BM_uint64ToAscii         9.13 ns         9.13 ns     74304737
// BM_u64ToAsciiTable       6.80 ns         6.80 ns     94725889
// BM_toString              90.5 ns         90.5 ns      7553911
// BM_abseilToString        5.19 ns         5.19 ns    124387049
// --------------------------------------------------------------

// const unsigned long long num = 9223372036854775800
// with compile flag -O2
// -------------------------------------------------------------
// Benchmark                   Time             CPU   Iterations
// -------------------------------------------------------------
// BM_uint64ToAscii         25.5 ns         25.5 ns     26020222
// BM_u64ToAsciiTable       17.6 ns         17.6 ns     38406349
// BM_toString               162 ns          162 ns      5198397
// BM_abseilToString        15.5 ns         15.5 ns     46544231
// BM_COu64toa              12.5 ns         12.5 ns     52603729
// -------------------------------------------------------------
// with compile flag -O3
// -------------------------------------------------------------
// BM_abseilToString        5.99 ns         5.99 ns    124352214
// BM_COu64toa              9.14 ns         9.14 ns     77551031
// -------------------------------------------------------------


