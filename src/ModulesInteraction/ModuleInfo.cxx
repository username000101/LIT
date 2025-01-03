#include <ModulesInteraction/ModulesInteraction.hxx>

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <Utils/Macros.hxx>

std::string lit::modules_interaction::ModuleInfo::name() const { return this->name_; }

std::string lit::modules_interaction::ModuleInfo::version() const { return this->version_; }

std::unordered_map<std::string, std::string> lit::modules_interaction::ModuleInfo::aliases()
const { return this->aliases_; }

std::string lit::modules_interaction::ModuleInfo::author() const { return this->author_; }

std::string lit::modules_interaction::ModuleInfo::description() const { return this->description_; }

std::filesystem::path lit::modules_interaction::ModuleInfo::module_path() const { return this->module_path_; }

std::optional<lit::modules_interaction::ModuleInfo> lit::modules_interaction::ModuleInfo::fromJson(const std::string& module_path, const std::string& json) {
    std::string name, version, author, description;
    std::unordered_map<std::string, std::string> aliases;
    static auto logger = spdlog::get("ModuleLoader");
    ASSERT(logger, "The 'ModuleLoader' logger is not initialized");

    auto config = nlohmann::json::parse(json);
    if (!config.contains("moduleName")) {
        logger->log(spdlog::level::err, "{}: The module config is invalid: the 'moduleName' field is missing", __FUNCTION__);
        return std::nullopt;
    }
    name = config.at("moduleName").get<std::string>();

    if (!config.contains("moduleVersion")) {
        logger->log(spdlog::level::warn, "{}: The module config is incomplete: the 'moduleVersion' field is missing", __FUNCTION__);
        version = "Undefined";
    } else
        version = config.at("moduleVersion").get<std::string>();

    if (!config.contains("moduleAuthor")) {
        logger->log(spdlog::level::warn, "{}: The module config is incomplete: the 'moduleAuthor' field is missing", __FUNCTION__);
        author = "Undefined";
    } else
        author = config.at("moduleAuthor").get<std::string>();

    if (!config.contains("moduleDescription")) {
        logger->log(spdlog::level::warn, "{}: The module config is incomplete: the 'moduleDescription' field is missing", __FUNCTION__);
        description = "Undefined";
    } else
        description = config.at("moduleDescription").get<std::string>();

    if (!config.contains("moduleAliases")) {
        logger->log(spdlog::level::err, "{}: The module config is invalid: the 'moduleAliases' field is missing", __FUNCTION__);
        return std::nullopt;
    }
    for (auto& alias : config.at("moduleAliases").items()) {
        if (!alias.value().is_string()) {
            logger->log(spdlog::level::warn, "{}: The module config is invalid: the 'moduleAliases' field contains invalid value(try to skip...)", __FUNCTION__);
        } else {
            aliases[alias.key()] = alias.value().get<std::string>();
        }
    }
    if (aliases.empty()) {
        logger->log(spdlog::level::err, "The module config is invalid: the 'moduleAliases' field is empty");
        return std::nullopt;
    } else {
        ModuleInfo result(module_path, name, version,
                             aliases, author, description);
        return std::optional<ModuleInfo>(result);
    }
}
