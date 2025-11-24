#pragma once
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include "sokol_time.h"
#include "tun/tstring.h"
#include "tun/tlist.h"

namespace tun {

class LogState {
public:
    String cerrLogPath {};
    String stderrLogPath {};
    std::ofstream logFile {};
    std::ofstream errorFile {};

    static LogState& Get() {
        static LogState state;
        return state;
    }

    LogState(const LogState&) = delete;
    LogState& operator=(const LogState&) = delete;

private:
    LogState() {}
};

class redirect_outputs
{
    std::ostream& myStream;
    std::streambuf *const myBuffer;
public:
    redirect_outputs ( std::ostream& lhs, std::ostream& rhs=std::cout )
        : myStream(rhs), myBuffer(myStream.rdbuf())
    {
        myStream.rdbuf(lhs.rdbuf());
    }

    ~redirect_outputs () {
        myStream.rdbuf(myBuffer);
    }
};

class capture_outputs
{
    std::ostringstream myContents;
    const redirect_outputs myRedirect;
public:
    capture_outputs ( std::ostream& stream=std::cout )
        : myContents(), myRedirect(myContents, stream)
    {}
    std::string contents () const
    {
        return (myContents.str());
    }
};

void InitLogging(StringView logPath, StringView errorLogPath, StringView cerrLogPath, StringView stderrLogPath);

template<typename ...Args>
inline String formatToString(std::format_string<Args...> fmt, Args&&... args) {
    return std::vformat(fmt.get(), std::make_format_args(args...));
}

struct LogTrace {
    uint64_t ticks {};
    bool enabled {true};
};

inline List<LogTrace> logStack {};

template<typename ...Args>
inline void log(std::format_string<Args...> fmt, Args&&... args) {
    for (int i = 0; i < logStack.size(); ++i) {
        std::cout << "  ";
    }
    std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}

template<typename ...Args>
inline void error(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << "[ERROR]: " << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}

inline void logpush(bool enabled = true) {
    logStack.push_back({stm_now(), enabled});
}

template<typename ...Args>
inline void logpop(std::format_string<Args...> fmt, Args&&... args) {
    const LogTrace trace = logStack.back();
    logStack.pop_back();

    uint64_t newTicks = stm_now();
    for (int i = 0; i < logStack.size(); ++i) {
        std::cout << "  ";
    }
    std::cout << formatToString("[{:.3f}ms]: ", stm_ms(stm_diff(newTicks, trace.ticks))) << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}

}
