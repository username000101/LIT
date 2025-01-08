#include <Configuration/LITConfiguration.hxx>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

lit::cfg::LITCfg lit::cfg::load_config(const std::filesystem::path& file) {
    static auto logger = spdlog::get("ConfigurationManager");
    ASSERT(logger, "The 'ConfigurationManager' logger is not initialized");

    if (!std::filesystem::exists(file)) {
        logger->log(spdlog::level::critical,
                    "{}: The configuration could not be loaded: the file does not exist(file=\"{}\")",
                    __PRETTY_FUNCTION__, file.string());
        std::abort();
    }

    std::ifstream file_stream(file);
    if (!file_stream.is_open()) {
        logger->log(spdlog::level::critical,
                    "{}: The configuration could not be loaded: failed to open file(file=\"{}\")",
                    __PRETTY_FUNCTION__, file.string());
        std::abort();
    }
    auto config = nlohmann::json::parse(file_stream);
    file_stream.close();

    std::string version, dir, api_id, api_hash;

    if (!config.contains("dir")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'dir' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }
    dir = config.at("dir").get<std::string>();

    if (!config.contains("version")) {
        logger->log(spdlog::level::warn,
                    "{}: The LIT configuration is incomplete (the 'version' field is missing), the 'LIT_VERSION' macro will be used",
                    __PRETTY_FUNCTION__);
        version =  LIT_VERSION;
    } else
        version = config.at("version").get<std::string>();

    if (!config.contains("td_settings")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'td_settings' object is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }
    auto config_td_settings = config.at("td_settings");

    if (!config_td_settings.contains("api_id")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'td_settings'::'api_id' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }
    api_id = config_td_settings.at("api_id").get<std::string>();

    if (!config_td_settings.contains("api_hash")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'td_settings'::'api_hash' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }
    api_hash = config_td_settings.at("api_hash").get<std::string>();

    if (!config.contains("modules")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'modules' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }

    return LITCfg(dir, version, api_id, api_hash);
}
