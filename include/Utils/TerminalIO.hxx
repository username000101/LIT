#ifndef TERMINALIO_HXX
#define TERMINALIO_HXX

#include <iostream>
#include <unordered_map>

namespace utils {
    namespace terminal_io {
        inline std::unordered_map<std::string, std::string> fgColor = {
            {"red", "\033[31m"},
            {"green", "\033[32m"},
            {"yellow", "\033[33m"},
            {"blue", "\033[34m"},
            {"reset", "\033[0m"},
        };

        inline std::unordered_map<std::string, std::string> bgColor = {
            {"red", "\033[41m"},
            {"green", "\033[42m"},
            {"yellow", "\033[43m"},
            {"blue", "\033[44m"},
            {"reset", "\033[0m"},
        };

        inline std::string console_input(const std::string& promt) {
            if (!promt.empty())
                std::cout << promt;
            std::string buffer;
            std::getline(std::cin, buffer);
            return buffer;
        }
    }
}

#endif //TERMINALIO_HXX
