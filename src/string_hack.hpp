#ifndef STRINGHACK_H
#define STRINGHACK_H
#include <string>
#include <sstream>

// from https://stackoverflow.com/questions/12975341/to-string-is-not-a-member-of-std-says-g-mingw
namespace std
{
template <typename T>
std::string to_string(const T &n)
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}
}

#endif
