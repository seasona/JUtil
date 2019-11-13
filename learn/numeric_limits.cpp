#include <iostream>
#include <limits>

int main() {
    std::cout << std::numeric_limits<int>::max() << std::endl;
    std::cout << std::numeric_limits<int>::min() << std::endl;
    // digits10是正确表示的十进制位数
    std::cout << std::numeric_limits<unsigned long long>::digits10 << std::endl;
    std::cout << std::numeric_limits<int>::digits10 << std::endl;
    return 0;
}