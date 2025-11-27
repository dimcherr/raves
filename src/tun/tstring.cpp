#include "tun/tstring.h"

StringView trimLeft(StringView sv) {
    auto start = sv.begin();
    while (start != sv.end() && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }
    return std::string_view(&(*start), sv.end() - start);
}

StringView trimRight(StringView sv) {
    auto end = sv.end();
    while (end != sv.begin() && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    return std::string_view(&(*sv.begin()), end - sv.begin());
}

StringView trim(StringView sv) {
    return trimRight(trimLeft(sv));
}

StringView substring(StringView sv, float percent) {
    return sv.substr(0, static_cast<int>(sv.size() * percent));
}

