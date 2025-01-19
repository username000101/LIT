#include <XLML/ModuleInstaller.hxx>

#include <iostream>
#include <string>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <Utils/Macros.hxx>

nlohmann::json lit::xlml::ModuleInstaller::read_and_merge_module_config(const std::string& config, const std::string& modules_config,
                                                                        const std::filesystem::path& module_path) {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    auto module_config_json = nlohmann::json::parse(config);
    auto global_config_json = nlohmann::json::parse(modules_config);
    nlohmann::json new_module_object = nlohmann::json::object();

    if (!module_config_json.contains("name")) {
        logger->log(spdlog::level::err,
                    "{}: Failed to merge configs: the module config not contains the 'name' field",
                    __PRETTY_FUNCTION__);
        return global_config_json;
    } else
        new_module_object["name"] = module_config_json.at("name").get<std::string>();

    if (!module_config_json.contains("version")) {
        logger->log(spdlog::level::warn,
                    "{}: The module config not contains the 'version' field, default 'version' is 'unknown'",
                    __PRETTY_FUNCTION__);
        new_module_object["version"] = "unknown";
    } else
        new_module_object["version"] = module_config_json.at("version").get<std::string>();

    if (!module_config_json.contains("author")) {
        logger->log(spdlog::level::warn,
                    "{}: The module config not contains the 'author' field, default 'author' is 'unknown'",
                    __PRETTY_FUNCTION__);
        new_module_object["author"] = "unknown";
    } else
        new_module_object["author"] = module_config_json.at("author").get<std::string>();

    if (!module_config_json.contains("description")) {
        logger->log(spdlog::level::warn,
                    "{}: The module config not contains the 'description' field, default 'description' is ''",
                    __PRETTY_FUNCTION__);
        new_module_object["description"] = "";
    } else
        new_module_object["description"] = module_config_json.at("description").get<std::string>();

    if (!module_config_json.contains("aliases")) {
        logger->log(spdlog::level::warn,
                    "{}: Failed to merge configs: the module config not contains the 'aliases' field",
                    __PRETTY_FUNCTION__);
        return global_config_json;
    } else
        for (auto& alias : module_config_json.at("aliases").items())
            new_module_object["aliases"][alias.key()] = alias.value();

    new_module_object["path"] = module_path.string();

    global_config_json["modules"][new_module_object.at("name").get<std::string>()] = new_module_object;
    return global_config_json;
}
