#include <Args/DefaultArguments.hxx>

#include <iostream>

#include <Filesystem/EnvironmentSetup.hxx>

std::unordered_map<std::string, void(*)(std::optional<std::vector<std::string>>)> lit::args::defargs::default_arguments = {
    {"--reinit", [](std::optional<std::vector<std::string>>) {
        std::cout << "Reinitialization..." << std::endl;
        if (std::filesystem::exists(LIT_CONFIG))
            std::filesystem::remove(LIT_CONFIG);
        if (std::filesystem::exists(LIT_MODULES_CONFIG))
            std::filesystem::remove(LIT_MODULES_CONFIG);
        lit::env::setup::setup_environment();
    }},
};
