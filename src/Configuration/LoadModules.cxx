#include <Configuration/LITConfiguration.hxx>

#include <dlfcn.h>

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <Utils/Libfn.hxx>
#include <Utils/Macros.hxx>
#include <RuntimeStorage/RuntimeStorage.hxx>
#include <ModulesInteraction/ModulesInteraction.hxx>

std::unordered_map<std::string, std::shared_ptr<lit::modules_interaction::ModuleInfo>> lit::cfg::load_modules() {
    static auto logger = spdlog::get("ConfigurationManager");
    ASSERT(logger, "The 'ConfigurationManager' logger is not initialized");

    if (!runtime_storage::LITConfig) {
        logger->log(spdlog::level::critical,
            "{}: You must first configure the basic configuration", __FUNCTION__);
        std::abort();
    }

    std::unordered_map<std::string, std::shared_ptr<modules_interaction::ModuleInfo>> modules;
    for (auto& module : runtime_storage::LITConfig->loadable_modules()) {
        void* module_handle = dlopen(module.second.c_str(), RTLD_LAZY);
        if (!module_handle) {
            logger->log(spdlog::level::err,
                "{}: Couldn't get the module's handle(it will be skipped)",
                __FUNCTION__);
        } else {
            auto config_function_ptr = utils::getFunction<std::string(*)()>(module_handle, "config");
            if (!config_function_ptr) {
                logger->log(spdlog::level::err,
                    "{}: Couldn't get a link to the function(it will be skipped)",
                    __FUNCTION__);
            } else {
                auto module_configuration = modules_interaction::ModuleInfo::fromJson
                (module.first, config_function_ptr());
                if (!module_configuration.has_value()) {
                    logger->log(spdlog::level::warn,
                        "{}: Failed to get ModuleInfo for module '{}'", __FUNCTION__, module.first);
                } else {
                    auto module_info = module_configuration.value();
                    modules[module_info.name()] = std::make_shared<modules_interaction::ModuleInfo>
                    (module.second, module_info.name(), module_info.version(),
                     module_info.aliases(), module_info.author(), module_info.description() );
                }
                /*
                try {

                    auto module_configuration = nlohmann::json::parse(config_function_ptr());
                    if (!module_configuration.contains("moduleAliases") ||
                        (module_configuration.contains("moduleAliases") &&
                         module_configuration.at("moduleAliases").empty())) {
                        logger->log(spdlog::level::critical,
                            "{}: Something strange: a module with an incorrect configuration got into loadable_modules",
                            __FUNCTION__);
                        std::abort();
                    } else {
                        std::unordered_map<std::string, std::string> module_commands;
                        for (auto& alias : module_configuration.at("moduleAliases").items()) {
                            module_commands[alias.key()] = alias.value().get<std::string>();
                        }

                        logger->log(spdlog::level::trace,
                            "{}: {} commands have been added for the '{}' module",
                            __FUNCTION__, module_commands.size(), std::string(module_configuration.at("moduleName").get<std::string>()));
                        modules[std::string(module_configuration.at("moduleName").get<std::string>())] = module_commands;
                    }
                } catch (boost::system::system_error& boost_syserr) {
                    logger->log(spdlog::level::warn,
                        "{}: Boost threw an exception: {}",
                        __FUNCTION__, boost_syserr.what());
                    continue;
                }
                */
            }
        }
        dlclose(module_handle);
    }
    return modules;
}
