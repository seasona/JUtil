#include <cstdio>
#include <string>

void print(int &num) { printf("%d\n", num); }

void printString(const std::string &s) { printf("%s\n", s.c_str()); }

int main() {
    int x = 1;
    int &y = x;
    print(y);
    // because trans a temporary variable to function, so the function argument
    // must be const reference
    printString(std::string("test"));
    return 0;
}