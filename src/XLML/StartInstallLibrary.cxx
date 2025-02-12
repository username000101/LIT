#include <XLML/ModuleInstaller.hxx>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>
#include <Utils/Libfn.hxx>

bool lit::xlml::ModuleInstaller::start_install_library(const std::filesystem::path& path) {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    auto handle = utils::open_library(path.string());
    if (!handle) {
        logger->log(spdlog::level::err,
                    "{}: Failed to open module '{}'",
                    __PRETTY_FUNCTION__,
                    path.string());
        return false;
    }

    auto config_fn = utils::get_symbol<const char*(*)()>(handle, "config");
    if (!config_fn) {
        logger->log(spdlog::level::err,
                    "{}: Failed to call 'config' function from module '{}'",
                    __PRETTY_FUNCTION__, 
                    path.string());
        utils::close_library(handle);
        return false;
    }

    std::ifstream confr(LIT_MODULES_CONFIG);
    if (!confr.is_open()) {
        logger->log(spdlog::level::err,
                    "{}: Failed to read LIT configuration module '{}': stream open error",
                    __PRETTY_FUNCTION__, path.string());
        return false;
    }
    auto gl_config = nlohmann::json::parse(confr);
    std::string module_config = config_fn();
    utils::close_library(handle);
    confr.close();

    std::filesystem::copy(path, LIT_MODULES_DIR / path.filename());
    std::filesystem::remove(path);
    logger->log(spdlog::level::debug,
                "{}: The module '{}' has been moved to '{}'",
                __PRETTY_FUNCTION__, path.string(),
                (LIT_MODULES_DIR / path.filename()).string());

    std::ofstream confw(LIT_MODULES_CONFIG, std::ios::trunc);
    if (!confw.is_open()) {
        logger->log(spdlog::level::err,
                    "{}: Failed to append configuration for module '{}': stream open error",
                    __PRETTY_FUNCTION__, path.string());
        return false;
    }
    confw << this->read_and_merge_module_config(module_config, gl_config.dump(), (LIT_MODULES_DIR / path.filename()).string()).dump(4);
    return false;
    confw.close();
}
