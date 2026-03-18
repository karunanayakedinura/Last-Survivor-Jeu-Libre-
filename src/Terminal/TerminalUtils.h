#pragma once
#include <string>

namespace Engine::Systems::TerminalUtils {
    enum class LogType { Info, Debug, Warning, Error, Success, Normal };

    struct LogMessage {
        std::string text;
        LogType type;
    };
} // namespace Engine::Systems::TerminalUtils