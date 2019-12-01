#include <chrono>
#include <iostream>

int main() {
    using std::chrono::duration_cast;

    /// time_since_epoch can support nanoseconds
    auto now = std::chrono::high_resolution_clock::now();
    std::cout
        << duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()
        << std::endl;

    std::cout << duration_cast<std::chrono::milliseconds>(
                     now.time_since_epoch())
                     .count()
              << std::endl;

    std::cout << duration_cast<std::chrono::microseconds>(
                     now.time_since_epoch())
                     .count()
              << std::endl;

    std::cout << duration_cast<std::chrono::nanoseconds>(now.time_since_epoch())
                     .count()
              << std::endl;

    return 0;
}
