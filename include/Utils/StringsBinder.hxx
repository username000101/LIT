#ifndef LIT_UTILS_STRINGSBINDER_HXX
#define LIT_UTILS_STRINGSBINDER_HXX

#include <iostream>
#include <string>
#include <sstream>

namespace utils {
    template <typename T> concept Stringable =
        requires(T data) {
            { std::cout << data } -> std::same_as<std::ostream&>;
        };

        template <Stringable... InputData> std::string bindStrings(InputData&&... data) {
            std::ostringstream oss;
            (oss << ... << data);
            return oss.str();
        }
}

#endif // LIT_UTILS_STRINGSBINDER_HXX
