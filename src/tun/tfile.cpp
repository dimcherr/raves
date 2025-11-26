#include "tun/tfile.h"
#include <cstddef>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <iostream>
#include <string>
#include "tun/tlog.h"

List<String> tfile::list(StringView path) {
    List<String> result {};
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        result.push_back(entry.path().filename().string());
    }
    return result;
}

String tfile::read(StringView path) {
    auto size = std::filesystem::file_size(path);
    String content(size, '\0');
    std::ifstream in(path.data());
    in.read(&content[0], size);
    return std::move(content);
}

List<Byte> tfile::readBinary(StringView path) {
    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        return {};
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    List<Byte> buffer(size);
    if (size > 0 && !file.read(buffer.data(), size)) {
        terror("Failed to read file: {}", path);
        return {};
    }
    
    return buffer;
}

bool tfile::write(StringView path, StringView content) {
    std::ofstream file(path.data());
    if (!file.is_open()) {
        return false;
    }
    if (content.size() > 0 && !file.write(content.data(), content.size())) {
        return false;
    }
    return true;
}
