#include <Args/CLArgs.hxx>

#include <spdlog/spdlog.h>

void lit::args::process_args(int argc, char** argv, std::unordered_map<std::string,
    void(*)(std::optional<std::vector<std::string>>)> args) {
    if (args.empty()) {
        spdlog::log(spdlog::level::warn, "{}: No arguments are set (all arguments will be skipped)", __PRETTY_FUNCTION__);
        return;
    }

    for (int i = 1; i < argc && argv[i]; ++i) {
        if (args.contains(argv[i]))
            args.at(argv[i])(std::nullopt);
        else
            spdlog::log(spdlog::level::warn,
                "The argument does not match any of the established ones (it will be skipped)");
    }
}
