#include <Args/DefaultArguments.hxx>

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <Filesystem/EnvironmentSetup.hxx>
#include <Utils/Libfn.hxx>
#include <Utils/StringsBinder.hxx>
#include <Utils/TerminalIO.hxx>
#include <Auth/TDLibAuthentification.hxx>

namespace lit {
    namespace modules_interaction {
        class TdWrap;
    }
}

auto info = [](std::optional<std::vector<std::string>> args) {
    std::cout << "Directory: " << LIT_DIR
              << "LIT version(by macros): " << LIT_VERSION << " " << LIT_VERSION_TYPE << std::endl;

    std::ifstream config_s(LIT_CONFIG);
    if (!config_s.is_open()) {
        std::cout << "Failed to open " << LIT_CONFIG << std::endl;
        std::exit(-1);
    } else {
        auto config = nlohmann::json::parse(config_s);
        config_s.close();
        if (config.contains("version"))
            std::cout << "LIT version(by config): " << config.at("version").get<std::string>() << std::endl;
    }

    std::ifstream modules_config(LIT_MODULES_CONFIG);
    if (!modules_config.is_open()) {
        std::cout << "Failed to open " << LIT_MODULES_CONFIG << std::endl;
        std::exit(-1);
    } else {
        auto modules_json = nlohmann::json::parse(modules_config);
        modules_config.close();
        if (!modules_json.contains("modules")) {
            std::cout << "The " << LIT_MODULES_CONFIG << " is incomplete(please start LIT with '--reinit' parameter" << std::endl;
            std::exit(-2);
        } else {
            std::cout << "Modules: " << modules_json.at("modules").size() << std::endl;
            std::exit(0);
        }
    }
};

auto install = [](std::optional<std::vector<std::string>> args) {
    if (!args.has_value()) {
        std::cout << "--install/-i usage: " << std::endl
                  << "[sudo] [./]LIT --install /path/to/library.so" << std::endl
                  << "(In this example, the module must contain a std::string config() function to get the configuration)" << std::endl << std::endl
                  << "[sudo] [./]LIT --install /path/to/library.so /path/to/config.json" << std::endl
                  << "(In this example you must have a configuration file for the module)" << std::endl;
        std::exit(-1);
    } else {
        if (args.value().size() == 1) {
            if (!std::filesystem::exists(args.value()[0])) {
                std::cout << "File not found: " << args.value()[0] << std::endl;
                std::exit(-2);
            }
           auto lib = utils::open_library(args.value()[0].c_str());
            if (!lib) {
                std::cout << "Failed to open module: " << args.value()[0] << std::endl;
                std::exit(-3);
            }

            auto config_function = utils::get_symbol<std::string(*)()>(lib, "config");
            if (!config_function) {
                std::cout << "Failed to sym 'config' function: " << args.value()[0] << std::endl;
                utils::close_library(lib);
                std::exit(-4);
            }

            auto module_config = nlohmann::json::parse(config_function());
            auto module_object = module_config.begin();

            std::string new_path = (std::filesystem::path(LIT_MODULES_DIR) / std::filesystem::path(args.value()[0]).filename()).string();
            std::filesystem::copy(args.value()[0], new_path, std::filesystem::copy_options::overwrite_existing);
            std::filesystem::remove(args.value()[0]);

            if (!module_object->contains("path"))
                module_object->push_back({"path", new_path});
            else
                module_object->at("path") = new_path;


            std::fstream modules_config(LIT_MODULES_CONFIG, std::ios::in);
            if (!modules_config.is_open()) {
                std::cout << "Failed to open modules config" << std::endl;
                std::exit(-5);
            }

            auto modules_config_j = nlohmann::json::parse(modules_config);
            modules_config.close();

            modules_config_j.at("modules")[module_object.key()] = module_object.value();
            modules_config.close();
            modules_config.open(LIT_MODULES_CONFIG, std::ios::out | std::ios::trunc);
            if (!modules_config.is_open()) {
                std::cout << "Failed to open modules config" << std::endl;
                std::exit(-6);
            }
            modules_config << modules_config_j.dump(4);
            modules_config.close();
            std::cout << "Module " << args.value()[0] << " has been installed" << std::endl;
            std::exit(0);
        } else if (args.value().size() == 2) {
            if (!std::filesystem::exists(args.value()[0])) {
                std::cout << "File not found: " << args.value()[0] << std::endl;
                std::exit(-7);
            } else if (!std::filesystem::exists(args.value()[1])) {
                std::cout << "File not found: " << args.value()[1] << std::endl;
                std::exit(-8);
            }

            std::ifstream module_config(args.value()[1]);
            if (!module_config.is_open()) {
                std::cout << "Failed to open module config: " << args.value()[1] << std::endl;
                std::exit(-9);
            }

            auto module_object = nlohmann::json::parse(module_config);
            auto module_config_j = module_object.begin();

            if (!module_config_j->contains("name")) {
                std::cout << "Invalid config: " << args.value()[1] << ": not found 'name' field" << std::endl;
                std::exit(-10);
            } else if (!module_config_j->contains("aliases")) {
                std::cout << "Invalid config: " << args.value()[1] << ": not found 'aliases' field" << std::endl;
                std::exit(-11);
            }

            auto lib = utils::open_library(args.value()[0].c_str());
            if (!lib) {
                std::cout << "Failed to open module: " << args.value()[0] << std::endl;
                std::exit(-12);
            }

            for (auto& alias : module_config_j->at("aliases").items()) {
                if (!utils::get_symbol<int(*)(std::shared_ptr<lit::modules_interaction::TdWrap>)>(lib, alias.value().get<std::string>())) {
                    std::cout << "Failed to install module: " << args.value()[0] << ": not found symbol '" << alias.value() << "'" << std::endl;
                    std::exit(-13);
                } else
                    std::cout << args.value()[0] << ": found symbol " << alias.value() << "" << std::endl;
            }

            utils::close_library(lib);

            std::string new_path = (std::filesystem::path(LIT_MODULES_DIR) / std::filesystem::path(args.value()[0]).filename()).string();
            std::filesystem::copy(args.value()[0], new_path, std::filesystem::copy_options::overwrite_existing);
            std::filesystem::remove(args.value()[0]);
            std::filesystem::remove(args.value()[1]);

            if (!module_config_j->contains("path"))
                module_config_j->push_back({"path", new_path});
            else
                module_config_j->at("path") = new_path;

            std::fstream modules_config(LIT_MODULES_CONFIG, std::ios::in);
            if (!modules_config.is_open()) {
                std::cout << "Failed to open modules configuration" << std::endl;
                std::exit(-14);
            }

            auto modules_config_j = nlohmann::json::parse(modules_config);
            modules_config.close();
            if (!modules_config_j.contains("modules")) {
                std::cout << "The " << LIT_MODULES_CONFIG << " is incomplete(please start LIT with '--reinit' parameter" << std::endl;
                std::exit(-15);
            }

            modules_config.open(LIT_MODULES_CONFIG, std::ios::out | std::ios::trunc);
            if (!modules_config.is_open()) {
                std::cout << "Failed to open modules config" << std::endl;
                std::exit(-16);
            }

            modules_config_j.at("modules")[module_config_j.key()] = module_config_j.value();
            modules_config << modules_config_j.dump(4);
            modules_config.close();
            std::cout << "Module " << args.value()[0] << " has been installed" << std::endl;
            std::exit(0);
        }
    }
};

std::unordered_map<std::string, void(*)(std::optional<std::vector<std::string>>)> lit::args::defargs::default_arguments = {
    {"--reinit", [](std::optional<std::vector<std::string>>) {
        std::cout << "Reinitialization..." << std::endl;
        if (std::filesystem::exists(LIT_CONFIG))
            std::filesystem::remove(LIT_CONFIG);
        if (std::filesystem::exists(LIT_MODULES_CONFIG))
            std::filesystem::remove(LIT_MODULES_CONFIG);
        lit::env::setup::setup_environment();
        std::exit(0);
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

    {"--info", info},
    {"-info", info},

    {"--install", install},
    {"-i", install},

    {"--auth", [](std::optional<std::vector<std::string>> args) {
        for (auto& file : std::filesystem::directory_iterator(LIT_TDLIB_FILES_DIR)) {
            std::cout << "Removing file " << file << std::endl;
            std::filesystem::remove(file);
        }

        for (auto& file : std::filesystem::directory_iterator(LIT_TDLIB_DATABASE_DIR)) {
            std::cout << "Removing file " << file << std::endl;
            std::filesystem::remove(file);
        }

        std::cout << "Current accout was removed, now just start LIT and auth again" << std::endl;
        std::exit(0);
     }},
};
