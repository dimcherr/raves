#pragma once
#include <format>
#include <iostream>
#include "sokol_time.h"
#include "tun/tstring.h"
#include "tun/tlist.h"

namespace tlogg {
    struct LogTrace {
        uint64_t ticks {};
        bool enabled {true};
    };

    inline List<LogTrace> logStack {};
}

template<typename ...Args>
inline String formatToString(std::format_string<Args...> fmt, Args&&... args) {
    return std::vformat(fmt.get(), std::make_format_args(args...));
}

template<typename ...Args>
inline void tlog(std::format_string<Args...> fmt, Args&&... args) {
    for (int i = 0; i < tlogg::logStack.size(); ++i) {
        std::cout << "  ";
    }
    std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}

template<typename ...Args>
inline void terror(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << "[ERROR]: " << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}

inline void tlogpush(bool enabled = true) {
    tlogg::logStack.push_back({stm_now(), enabled});
}

template<typename ...Args>
inline void tlogpop(std::format_string<Args...> fmt, Args&&... args) {
    const tlogg::LogTrace trace = tlogg::logStack.back();
    tlogg::logStack.pop_back();

    uint64_t newTicks = stm_now();
    for (int i = 0; i < tlogg::logStack.size(); ++i) {
        std::cout << "  ";
    }
    std::cout << formatToString("[{:.3f}ms]: ", stm_ms(stm_diff(newTicks, trace.ticks))) << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}

