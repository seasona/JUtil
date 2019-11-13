/// Andrei alexandrescu three optimization tips for c++ 提到一个格式化数字为字符串的优化
/// 因为整数除法很慢，所以每次除以两位，并到映射中查找来节省除法的时间，可以看到每次除以两位的速度几乎
/// 是普通每次除一位方法的两倍，更是to_string的5倍以上，这是一种比较好的优化方法 

#include <benchmark/benchmark.h>
#include <stdint.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>

const unsigned long long num = 9223372036854775800;
// const int num = 82635722;
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

BENCHMARK(BM_uint64ToAscii);
BENCHMARK(BM_u64ToAsciiTable);
BENCHMARK(BM_toString);
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
