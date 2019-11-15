#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

void learn_time_point() {
    std::chrono::time_point now = std::chrono::high_resolution_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::cout << "The time of now is: "
              << std::put_time(std::localtime(&time), "%F %T") << std::endl;
}

/// time_since_epoch support accuracy of nanosecond
void learn_time_since_epoch() {
    using std::chrono::duration_cast;

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
}