#ifndef LIT_ARGS_DEFAULTARGUMENTS_HXX
#define LIT_ARGS_DEFAULTARGUMENTS_HXX

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace lit {
    namespace args {
        namespace defargs {
            extern std::unordered_map<std::string, void(*)(std::optional<std::vector<std::string>>)> default_arguments;

        }

    }
}

#endif // LIT_ARGS_DEFAULTARGUMENTS_HXX