#pragma once
#include <limits>
#include <string>

static const char digits[] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

class FormatInt {
public:
    explicit FormatInt(int value) { formatSigned(value); }
    explicit FormatInt(long value) { formatSigned(value); }
    explicit FormatInt(long long value) { formatSigned(value); }
    explicit FormatInt(unsigned value) : str_(formatDecimal(value)) {}
    explicit FormatInt(unsigned long value) : str_(formatDecimal(value)) {}
    explicit FormatInt(unsigned long long value) : str_(formatDecimal(value)) {}

    std::size_t size() { return (buffer_ - str_ + buffer_size - 1); }

    const char *data() const { return str_; }

    const char *c_str() const {
        buffer_[buffer_size - 1] = '\0';
        return str_;
    }

    std::string str() const {
        return std::string(str_, buffer_ - str_ + buffer_size - 1);
    }

private:
    enum {
        buffer_size = std::numeric_limits<unsigned long long>::digits10 + 3
    };
    mutable char buffer_[buffer_size];
    char *str_;

    char *formatDecimal(unsigned long long value) {
        char *ptr = buffer_ + buffer_size - 1;

        while (value > 100) {
            unsigned index = static_cast<unsigned>(value % 100 * 2);
            value /= 100;
            *(--ptr) = digits[index + 1];
            *(--ptr) = digits[index];
        }
        if (value > 10) {
            unsigned index = static_cast<unsigned>(value * 2);
            *(--ptr) = digits[index + 1];
            *(--ptr) = digits[index];
        } else {
            *(--ptr) = static_cast<char>('0' + value);
        }
        return ptr;
    }

    void formatSigned(long long value) {
        if (value < 0) {
            unsigned long long abs_value = std::abs(value);
            str_ = formatDecimal(abs_value);
            *--str_ = '-';
        } else {
            str_ = formatDecimal(value);
        }
    }
};
