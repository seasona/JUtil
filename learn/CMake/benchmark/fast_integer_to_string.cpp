/// Andrei alexandrescu three optimization tips for c++
/// 提到一个格式化数字为字符串的优化
/// 因为整数除法很慢，所以每次除以两位，并到映射中查找来节省除法的时间，可以看到每次除以两位的速度几乎
/// 是普通每次除一位方法的两倍，更是to_string的5倍以上，这是一种比较好的优化方法

#include <benchmark/benchmark.h>
#include <stdint.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>

//const unsigned long long num = 9223372036854775800;
const unsigned int num = 82635722;
enum { buffer_size = std::numeric_limits<int>::digits10 + 3 };
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

template <typename T>
unsigned u64ToAsciiTable(T value, char* dst) {
    static const char digits[201] =
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";

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

const char one_ASCII_final_digits[10][2] {
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

static void BM_abseilToString(benchmark::State& state){
    for(auto _: state){
        FastIntToBuffer(num, str);
    }
}

BENCHMARK(BM_uint64ToAscii);
BENCHMARK(BM_u64ToAsciiTable);
BENCHMARK(BM_toString);
BENCHMARK(BM_abseilToString);
BENCHMARK_MAIN();

// const int num = 82635722
// -------------------------------------------------------------
// Benchmark                   Time             CPU   Iterations
// -------------------------------------------------------------
// BM_uint64ToAscii         40.2 ns         40.2 ns     17285239
// BM_u64ToAsciiTable       17.8 ns         17.8 ns     39675770
// BM_toString               166 ns          166 ns      4122648
// -------------------------------------------------------------

// const unsigned long long num = 9223372036854775800
// -------------------------------------------------------------
// Benchmark                   Time             CPU   Iterations
// -------------------------------------------------------------
// BM_uint64ToAscii         98.9 ns         98.9 ns      6026728
// BM_u64ToAsciiTable       44.7 ns         44.7 ns     15425931
// BM_toString               245 ns          245 ns      2841475
// -------------------------------------------------------------
