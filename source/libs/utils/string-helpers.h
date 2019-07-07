#ifndef UTILS_STRING_HELPERS_H
#define UTILS_STRING_HELPERS_H

#include <string>

namespace utils {

    const char *ws = " \t\n\r\f\v";

    inline std::string &rtrim(std::string &s, const char *t = ws) {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    inline std::string &ltrim(std::string &s, const char *t = ws) {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    inline std::string &trim(std::string &s, const char *t = ws) {
        return ltrim(rtrim(s, t), t);
    }

}  // namespace utils

#endif  // UTILS_STRING_HELPERS_H
