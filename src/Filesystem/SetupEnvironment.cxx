#include <Filesystem/EnvironmentSetup.hxx>

#include <filesystem>
#include <fstream>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

void lit::env::setup::setup_environment(const std::filesystem::path& PREFIX) {
    if (!std::filesystem::exists(PREFIX.string())) {
        spdlog::log(spdlog::level::info,
                    "{}: The LIT files directory could not be found (perhaps it is being launched for the first time?). It will be created automatically",
                    __PRETTY_FUNCTION__);

        std::filesystem::create_directories(PREFIX.string());
        std::filesystem::create_directory(PREFIX.string() + "/files");
        std::filesystem::create_directory(PREFIX.string() + "/modules");
        std::filesystem::create_directory(PREFIX.string() + "/translations");
        std::filesystem::create_directory(PREFIX.string() + "/logs");
    } else {
        if (!std::filesystem::exists(PREFIX.string() + "/files"))
            std::filesystem::create_directory(PREFIX.string() + "/files");
        else if (!std::filesystem::exists(PREFIX.string() + "/modules"))
            std::filesystem::create_directory(PREFIX.string() + "/modules");
        else if (!std::filesystem::exists(PREFIX.string() + "/translations"))
            std::filesystem::create_directory(PREFIX.string() + "/translations");
        else if (!std::filesystem::create_directory(PREFIX.string() + "/logs"))
            std::filesystem::create_directory(PREFIX.string() + "/logs");
        if (!std::filesystem::exists(PREFIX.string() + "/tdlib")) {
            std::filesystem::create_directory(PREFIX.string() + "/tdlib");
            std::filesystem::create_directory(PREFIX.string() + "/tdlib/db");
            std::filesystem::create_directory(PREFIX.string() + "/tdlib/files");
        }
        else
            spdlog::log(spdlog::level::info,
                        "{}: There is no need to create directories, everything is fine",
                        __PRETTY_FUNCTION__);
    }

    if (!std::filesystem::exists(PREFIX.string() + "/modules.json")) {
        std::ofstream modules_config_file(PREFIX.string() + "/modules.json");
        if (!modules_config_file.is_open()) {
            spdlog::log(spdlog::level::critical,
                        "{}: Failed to create '{}/modules.json': stream open error",
                        __PRETTY_FUNCTION__, 
                        PREFIX.string());
            std::abort();
        }
        modules_config_file << R"({
    "modules": {

    }
})";
        modules_config_file.close();
    }

    if (!std::filesystem::exists(PREFIX.string() + "/config.json")) {
        std::ofstream config_file(PREFIX.string() + "/config.json");
        if (!config_file.is_open()) {
            spdlog::log(spdlog::level::critical,
                        "{}: Failed to create '{}/config.json': stream open error",
                        __PRETTY_FUNCTION__, 
                        PREFIX.string());
        }
        auto prefix_str = PREFIX.string();
        config_file << fmt::format(R"({{
    "dir": "{}",
    "version": "{}",
    "td_settings": {{
        "api_id": "{}",
        "api_hash": "{}"
    }},
    "modules": "{}"
}}
            )",
        PREFIX.string(), LIT_VERSION, LIT_API_ID, LIT_API_HASH, LIT_MODULES_CONFIG) << std::endl;
        config_file.close();
    }
}

