#include <Args/DefaultArguments.hxx>

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <Filesystem/EnvironmentSetup.hxx>
#include <Utils/StringsBinder.hxx>
#include <Utils/TerminalIO.hxx>

std::unordered_map<std::string, void(*)(std::optional<std::vector<std::string>>)> lit::args::defargs::default_arguments = {
    {"--reinit", [](std::optional<std::vector<std::string>>) {
        std::cout << "Reinitialization..." << std::endl;
        if (std::filesystem::exists(LIT_CONFIG))
            std::filesystem::remove(LIT_CONFIG);
        if (std::filesystem::exists(LIT_MODULES_CONFIG))
            std::filesystem::remove(LIT_MODULES_CONFIG);
        lit::env::setup::setup_environment();
    }},

    {"--writeconf", [](std::optional<std::vector<std::string>>) {
        std::ifstream read_config(LIT_CONFIG);
        if (!read_config.is_open()) {
            std::cout << "Fatal: failed to open configuration file: read stream open error" << std::endl;
            std::exit(-1);
        }

        auto config = nlohmann::json::parse(read_config);
        read_config.close();

        std::ofstream write_config(LIT_CONFIG, std::ios::out | std::ios::trunc);
        if (!write_config.is_open()) {
            std::cout << "Fatal: failed to open configuration file: write stream open error" << std::endl;
            std::exit(-2);
        }

        auto key_name = utils::terminal_io::console_input("Key name>");
        if (config.contains(key_name)) {
            if (utils::terminal_io::console_choice(
                    utils::bindStrings("The key '", key_name, "' already has value(", config.at(key_name).get<std::string>(), "), overwrite it?"),
                    {"y", "n"}) == "n")
                std::exit(-3);
            else {
                auto new_key_value = utils::terminal_io::console_input("New key value>");
                if (new_key_value == config.at(key_name).get<std::string>()) {
                    std::cout << "The value can't changed: values are same" << std::endl;
                    std::exit(-4);
                } else {
                    config.at(key_name) = new_key_value;
                    std::cout << "Now the configuration file looks this:\n" << config.dump(4) << std::endl;
                    if (utils::terminal_io::console_choice(
                            "DO YOU REALLY WANT TO CHANGE CONFIGURATION FILE?("
                            "In which case, you can always restore the LIT configuration by running it with the '--reinit' argument."
                            ")",
                            {"y", "n"}) == "y") {
                        write_config << config.dump(4);
                        write_config.close();
                        std::cout << "Ok" << std::endl;
                        std::exit(0);
                    } else {
                        std::cout << "Changes restored" << std::endl;
                        std::exit(0);
                    }
                }
            }
        } else {
            auto key_value = utils::terminal_io::console_input("Key value>");
            config[key_name] = key_value;
            std::cout << "Now the configuration file looks this:\n" << config.dump(4) << std::endl;
            if (utils::terminal_io::console_choice(
                    "DO YOU REALLY WANT TO CHANGE CONFIGURATION FILE?("
                    "In which case, you can always restore the LIT configuration by running it with the '--reinit' argument."
                    ")",
                    {"y", "n"}) == "y") {
                write_config << config.dump(4);
                write_config.close();
                std::cout << "Ok" << std::endl;
                std::exit(0);
            } else {
                std::cout << "Changes restored" << std::endl;
                std::exit(0);
            }
        }
    }},
};
