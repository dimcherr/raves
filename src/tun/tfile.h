#pragma once
#include <cstdint>
#include "tun/tlist.h"
#include "tun/tstring.h"

using Byte = char;

namespace tfile {
    String read(StringView path);
    List<Byte> readBinary(StringView path);
    bool write(StringView path, StringView content);
    List<String> list(StringView path);
}
