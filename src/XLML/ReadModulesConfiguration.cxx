#include <XLML/ModulesConfigurationReader.hxx>

#include <filesystem>
#include <fstream>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

std::unordered_map<std::string, lit::modules_interaction::ModuleInfo> lit::xlml::read_modules_configuration(
    const std::filesystem::path& modules_configuration_file) {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    if (!std::filesystem::exists(modules_configuration_file)) {
        logger->log(spdlog::level::critical,
                    "{}: Failed to read modules configuration: file '{}' doesn't exists",
                    __PRETTY_FUNCTION__, 
                    modules_configuration_file.string());
        std::abort();
    }

    std::ifstream read_file(modules_configuration_file);
    if (!read_file.is_open()) {
        logger->log(spdlog::level::critical,
                    "{}: Failed to read modules configuration: stream open error",
                    __PRETTY_FUNCTION__);
        std::abort();
    }

    std::unordered_map<std::string, modules_interaction::ModuleInfo> result;
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(read_file);
    } catch (std::exception& err) {
        logger->log(spdlog::level::err,
                    "{}: It looks like there are problems reading the configuration file: {}",
                    __PRETTY_FUNCTION__, err.what());
        std::exit(-1);
    }
    read_file.close();

    if (!json.contains("modules") || json["modules"].empty()) {
        logger->log(spdlog::level::warn,
                    "{}: The modules configuration is empty(running without modules, only built-in functions)",
                    __PRETTY_FUNCTION__);
        return {};
    }

    for (auto& module : json.at("modules").items()) {
        std::filesystem::path path;
        std::string name, version, author, description;
        std::unordered_map<std::string, std::string> aliases;

        if (!module.value().contains("path")) {
            logger->log(spdlog::level::warn,
                        "{}: The module is invalid(the 'path' field is missing), it will be skipped",
                        __PRETTY_FUNCTION__);
            continue;
        }
        path = module.value().at("path").get<std::string>();
        if (!std::filesystem::exists(path)) {
            logger->log(spdlog::level::warn,
                        "{}: Invalid path for module: {}",
                        __PRETTY_FUNCTION__, 
                        path.string());
            continue;
        }

        if (!module.value().contains("name")) {
            logger->log(spdlog::level::warn,
                        "{}: The module is invalid(the 'name' field is missing), it will be skipped",
                        __PRETTY_FUNCTION__);
            continue;
        }
        name = module.value().at("name").get<std::string>();

        if (!module.value().contains("version")) {
            logger->log(spdlog::level::warn,
                        "{}: The module is incomplete(the 'version' field is missing), default version is 'unknown'",
                        __PRETTY_FUNCTION__);
            version = "unknown";
        } else
            version = module.value().at("version").get<std::string>();

        if (!module.value().contains("author")) {
            logger->log(spdlog::level::warn,
                        "{}: The module is incomplete(the 'author' field is missing), default author is 'unknown'",
                        __PRETTY_FUNCTION__);
            author = "unknown";
        } else
            author = module.value().at("author").get<std::string>();

        if (!module.value().contains("description")) {
            logger->log(spdlog::level::warn,
                        "{}: The module is incomplete(the 'description' field is missing), default description is ''",
                        __PRETTY_FUNCTION__);
            description = "";
        } else
            description = module.value().at("description").get<std::string>();

        if (!module.value().contains("aliases")) {
            logger->log(spdlog::level::warn,
                        "{}: The module is invalid(the 'aliases' field is missing), it will be skipped",
                        __PRETTY_FUNCTION__);
            continue;
        }
        for (auto& alias : module.value().at("aliases").items()) {
            aliases[alias.key()] = alias.value().get<std::string>();
        }
        logger->log(spdlog::level::debug,
                    "{}: {} aliases has been loaded for module '{}'",
                    __PRETTY_FUNCTION__, 
                    aliases.size(), 
                    path.string());

        for (auto& alias : aliases) {
            logger->log(spdlog::level::debug,
                        "{}: Register alias '{}' for module '{}'",
                        __PRETTY_FUNCTION__, alias.first, name);
            result[alias.first] = std::move(modules_interaction::ModuleInfo(path, name, version, aliases, author, description));
        }
    }

    return result;
}
