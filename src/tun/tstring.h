#pragma once
#include <string>
#include <string_view>

using String = std::string;
using StringView = std::string_view;

StringView TrimLeft(StringView sv);
StringView TrimRight(StringView sv);
StringView Trim(StringView sv);
StringView Substring(StringView sv, float percent);
