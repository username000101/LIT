#ifndef LIT_ARGS_CLARGS_HXX
#define LIT_ARGS_CLARGS_HXX

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <Args/DefaultArguments.hxx>

namespace lit {
    namespace args {

        void process_args(int argc, char** argv, std::unordered_map<std::string,
            void(*)(std::optional<std::vector<std::string>>)> args = defargs::default_arguments);
    }
}

#endif // LIT_ARGS_CLARGS_HXX
