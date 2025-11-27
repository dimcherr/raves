#pragma once
#include <array>
#include <vector>
#include <unordered_map>

template <typename T, std::size_t size>
using Array = std::array<T, size>;
template <typename T>
using List = std::vector<T>;
template <typename T, typename U>
using Map = std::unordered_map<T, U>;

