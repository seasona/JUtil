#include <iostream>
#include <string>
#include <string_view>
//#include "benchmark/benchmark.h"

// static void StringSubstr(benchmark::State& state){
//     std::string s="Test of the performance of std::string's substr";
//     for(auto _:state){
//         auto str = s.substr(1,20);
//         benchmark::DoNotOptimize(str);
//     }
// }

// static void StringViewSubstr(benchmark::State& state){
//     std::string s="Test of the performance of std::string's substr";
//     for(auto _:state){
//         std::string_view sv=s;
//         auto str = sv.substr(1,20);
//         benchmark::DoNotOptimize(str);
//     }
// }

// BENCHMARK(StringSubstr);
// BENCHMARK(StringViewSubstr);
// BENCHMARK_MAIN();

// std::string_view's substr is much faster than std::string
// -----------------------------------------------------------
// Benchmark                 Time             CPU   Iterations
// -----------------------------------------------------------
// StringSubstr           24.5 ns         24.5 ns     30402415
// StringViewSubstr       1.06 ns         1.06 ns    606217924

// the std::string_view can transport argument both std::string and const char*
/// notice: do not use reference of string_view in argument
void stringViewInterface(std::string_view sv) { std::cout << sv << std::endl; }

int main() {
    const char *s1 = "this is const char";
    std::string s2{"this is std::string"};
    stringViewInterface(s1);
    stringViewInterface(s2);

    return 0;
}

// Pros:
// It provides a more flexible interface, can accept both const char* and const
// string&. 
// It’s faster than std::string, in some cases.

// Cons:
// It is not necessarily NUL-terminated, so in some case like printf it’s not
// safe. 
// The source data of the string_view must outlive the string_view itself.
