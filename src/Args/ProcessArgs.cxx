#include <Args/CLArgs.hxx>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

void lit::args::process_args(int argc, char** argv, std::unordered_map<std::string,
    void(*)(std::optional<std::vector<std::string>>)> args) {
    if (args.empty()) {
        spdlog::log(spdlog::level::warn, 
                    "{}: No arguments are set (all arguments will be skipped)",
                    __PRETTY_FUNCTION__);
        return;
    }

    for (int i = 1; i < argc && argv[i]; ++i) {
        if (args.contains(argv[i])) {
            std::vector<std::string> arguments;
            for (int j = i + 1; j < argc && argv[j]; ++j)
                arguments.push_back(argv[j]);

            if (arguments.empty())
                args.at(argv[i])(std::nullopt);
            else
                args.at(argv[i])(arguments);
        }
        else
            spdlog::log(spdlog::level::warn,
                        "{}: The argument does not match any of the established ones (it will be skipped)",
                        __PRETTY_FUNCTION__);
    }
}
