#include <Configuration/LITConfiguration.hxx>

#include <filesystem>
#include <fstream>
#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
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
    std::vector<td::td_api::int53> blocked_requests;

    if (!config.contains("dir")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'dir' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }

    // ========== Main ===========
    dir = config.at("dir").get<std::string>();

    [[unlikely]] if (!config.contains("version")) {
        logger->log(spdlog::level::warn,
                    "{}: The LIT configuration is incomplete (the 'version' field is missing), the 'LIT_VERSION' macro will be used",
                    __PRETTY_FUNCTION__);
        version =  LIT_VERSION;
    } else {
        if (config.at("version").get<std::string>() != LIT_VERSION)
        logger->log(spdlog::level::warn,
                    "{}: LIT version differs between config file and macro",
                    __PRETTY_FUNCTION__);
        version = config.at("version").get<std::string>();
    }

    [[unlikely]] if (!config.contains("td_settings")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'td_settings' object is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }

    // ========== Main::td_settings ==========
    auto config_td_settings = config.at("td_settings");

    [[unlikely]] if (!config_td_settings.contains("api_id")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'td_settings'::'api_id' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }
    api_id = config_td_settings.at("api_id").get<std::string>();

    [[unlikely]] if (!config_td_settings.contains("api_hash")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'td_settings'::'api_hash' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }
    api_hash = config_td_settings.at("api_hash").get<std::string>();

    [[unlikely]] if (config_td_settings.contains("blocked_requests")) {
        for (auto& blocked_request : config_td_settings.at("blocked_requests")){
            logger->log(spdlog::level::info,
                        "{}: A new blocked request has been added: {}",
                        __PRETTY_FUNCTION__, blocked_request.get<td::td_api::int53>());
            blocked_requests.push_back(blocked_request.get<td::td_api::int53>());
        }
    }

    [[unlikely]] if (!config.contains("modules")) {
        logger->log(spdlog::level::critical,
                    "{}: The LIT configuration is corrupted (the 'modules' field is missing)",
                    __PRETTY_FUNCTION__);
        std::abort();
    }

    // =========== Appearance ============
    [[unlikely]] if (config.contains("appearance")) {
        auto app = config.at("appearance");

        [[unlikely]] if (app.contains("command_prefix")) {
            logger->log(spdlog::level::info,
                        "{}: Redefining the command prefix: '{}' ==> '{}'",
                        __PRETTY_FUNCTION__, runtime_storage::vars::command_prefix,
                        app.at("command_prefix").get<std::string>());
            runtime_storage::vars::command_prefix = app.at("command_prefix").get<std::string>();
        }
    }

    return LITCfg(dir, version, api_id, api_hash, blocked_requests);
}
