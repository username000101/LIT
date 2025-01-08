#include <XLML/ModuleInsaller.hxx>

#include <filesystem>

#include <Utils/Macros.hxx>

bool lit::xlml::ModuleInstaller::start_install_archive(const std::filesystem::path& module) {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    logger->log(spdlog::level::info,
                "{}: This feature is not implemented yet, return false",
                __PRETTY_FUNCTION__);
    return false;
}
