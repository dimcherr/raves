#pragma once
#include "tun/tstring.h"
#include "tun/tlist.h"

namespace lang {

enum Lang : size_t {
    eng,
    rus,
};

extern Lang current;

}

namespace astring {

struct LocString {
    Array<String, 2> locales {};

    StringView Get() const {
        return locales[lang::current];
    }

    operator StringView() const {
        return Get();
    }
};

}