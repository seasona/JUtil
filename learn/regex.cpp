#include <iostream>
#include <regex>
#include <string>

std::basic_regex<char> option_specifier(
    "(([[:alnum:]]),)?[ ]*([[:alnum:]][-_[:alnum:]]*)?");

int main() {
    // std::match_results用于存储正则匹配的结果，result[0]存储完整匹配，后续的存储用()分割的子匹配
    std::match_results<const char*> result;
    std::string s = "s,special";
    std::regex_match(s.c_str(), result, option_specifier);

    std::cout << result[0] << "\n"   // s,special
              << result[1] << "\n"   // s,
              << result[2] << "\n"   // s
              << result[3] << "\n";  // special

    return 0;
}