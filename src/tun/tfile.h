#pragma once
#include <cstdint>
#include "tun/tlist.h"
#include "tun/tstring.h"

using Byte = char;
using Bytes = List<Byte>;

namespace tun {

String ReadFile(StringView path);
Bytes ReadFileBinary(StringView path);
bool WriteFile(StringView path, StringView content);
List<String> ListFiles(StringView path);

}