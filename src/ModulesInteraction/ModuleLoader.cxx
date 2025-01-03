#include <ModulesInteraction/ModulesInteraction.hxx>

#include <dlfcn.h>

#include <cassert>
#include <csignal>
#include <vector>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

std::vector<bool> lit::modules_interaction::ModuleLoader::try_functions(std::vector<std::string>&& functions) {
    static auto logger = spdlog::get("ModuleLoader");
    ASSERT(logger, "The 'ModuleLoader' logger is not initialized");

    if (functions.empty())
        return {};

    if (!this->library_) {
        logger->log(spdlog::level::err, "{}: Failed to try functions: the library_(loaded library handle) is not valid(maybe library is not opened?)", __FUNCTION__);
        std::raise(SIGUSR1);
    }

    std::vector<bool> result(functions.size());
    std::for_each(functions.begin(), functions.end(), [this, &result](const std::string& function) -> void {
        typedef void(*function_type)();
        function_type f = (function_type)dlsym(this->library_, function.c_str());
        if (!f) {
            logger->log(spdlog::level::err, "{}: Failed to try function '{}'(maybe function is not implemented?)", __FUNCTION__, function);
            result.emplace_back(false);
        }
        else
            result.emplace_back(true);
    });
    return result;
}

std::optional<lit::modules_interaction::ModuleLoader::ModuleCommands> lit::modules_interaction::ModuleLoader::load_module() {
    static auto logger = spdlog::get("ModuleLoader");
    ASSERT(logger, "The 'ModuleLoader' logger is not initialized");

    if (this->module_info_->module_path().empty() || !std::filesystem::exists(this->module_info_->module_path())) {
        logger->log(spdlog::level::err, "{}: Failed to load module: the module path '{}' is invalid", __FUNCTION__, this->module_info_->module_path().string());
        return std::nullopt;
    }
    this->library_ = dlopen(this->module_info_->module_path().c_str(), RTLD_NOW);
    if (!this->library_) {
        logger->log(spdlog::level::err,
            "{}: Failed to load module {}: library open error", __FUNCTION__, this->module_info_->name());
        return std::nullopt;
    }

    typedef int(*startf)(void(*)(const std::string&));
    typedef int(*stopf)();
    typedef std::string(*configf)();

    auto start_function = (startf)dlsym(this->library_, "start");
    auto stop_function = (stopf)dlsym(this->library_, "stop");
    auto config_function = (configf)dlsym(this->library_, "config");

    if (!start_function || !stop_function || !config_function) {
        logger->log(spdlog::level::err, "{}: Failed to load module '{}': some functions are not implemented", __FUNCTION__, this->module_info_->name());
        return std::nullopt;
    }

    ModuleCommands commands(start_function, stop_function, config_function);
    return commands;
}
