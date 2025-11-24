#include "tun/tlog.h"

void tun::InitLogging(StringView logPath, StringView errorLogPath, StringView cerrLogPath, StringView stderrLogPath) {
    auto& logState = tun::LogState::Get();
    logState.cerrLogPath = cerrLogPath;
    logState.stderrLogPath = stderrLogPath;

    std::ofstream log_file(cerrLogPath.data(), std::ios::binary);
    if (!log_file.is_open()) {
        std::cerr
            << "Could not open log file for writing."
            << std::endl;
        return;
    }
    const tun::redirect_outputs _(log_file, std::cerr);
    freopen(stderrLogPath.data(), "a", stderr);

    logState.logFile = std::ofstream(logPath.data());
    logState.errorFile = std::ofstream(errorLogPath.data());
}