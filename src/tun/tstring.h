#pragma once
#include <string>
#include <string_view>

using String = std::string;
using StringView = std::string_view;

StringView trimLeft(StringView sv);
StringView trimRight(StringView sv);
StringView trim(StringView sv);
StringView substring(StringView sv, float percent);

