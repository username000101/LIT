#include <XLML/ModuleInsaller.hxx>

#include <Utils/Macros.hxx>

void lit::xlml::ModuleInstaller::start_install_archive() {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    if (this->module_type_ == INVALID) {
        logger->log(spdlog::level::err,
            "{}: Failed to install module: the module_type_ == INVALID",
            __FUNCTION__);
        return;
    }


}
