#include <XLML/ExecModule.hxx>


#include <cstdlib>
#include <filesystem>
#include <string>
#include <memory>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include <Utils/Access.hxx>
#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/Macros.hxx>
#include <Utils/Libfn.hxx>
#include <ModulesInteraction/TDWrap.hxx>
#include <TelegramAPIInteraction/Updates.hxx>

bool lit::xlml::start_module(std::string command, std::vector<std::string> args,
                             std::shared_ptr<td::td_api::message> message) {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    if (runtime_storage::LITModules->count(command) < 1) {
        logger->log(spdlog::level::err,
                    "{}: Not found module for command '{}'",
                    __PRETTY_FUNCTION__, command);
        return false;
    }

    auto module_info = runtime_storage::LITModules->at(command);
    if (!std::filesystem::exists(module_info.module_path())) {
        logger->log(spdlog::level::err,
                    "{}: It looks like the module '{}' was deleted\\moved during execution.",
                    __PRETTY_FUNCTION__, module_info.name());
        return false;
    }

    std::string alias_function = module_info.aliases().at(command);

    void* handle = utils::open_library(module_info.module_path().string());
    if (!handle) {
        logger->log(spdlog::level::err,
                    "{}: Couldn't open the module",
                    __PRETTY_FUNCTION__);
        return false;
    }

    auto module_tdwrap_layer_version = utils::get_symbol<std::tuple<int, int, int>*>(handle, "TDWRAP_LAYER_VERSION");
    if (!module_tdwrap_layer_version) {
        logger->log(spdlog::level::warn,
                    "{}: The '{}' module does not contain a version of the TDWrap layer, the module may not be compatible.",
                    __PRETTY_FUNCTION__, module_info.name());
    } else
        if (*module_tdwrap_layer_version != TDWRAP_LAYER_VERSION) {
            logger->log(spdlog::level::err,
                    "{}: The TDWrap layers are different(LIT='{}.{}.{}';{}='{}.{}.{}'), the '{}' module  cannot be loaded",
                    __PRETTY_FUNCTION__,
                    std::get<0>(TDWRAP_LAYER_VERSION),
                    std::get<1>(TDWRAP_LAYER_VERSION),
                    std::get<2>(TDWRAP_LAYER_VERSION),
                    module_info.name(),
                    std::get<0>(*module_tdwrap_layer_version),
                    std::get<1>(*module_tdwrap_layer_version),
                    std::get<2>(*module_tdwrap_layer_version),
                    module_info.name());
            return false;
        }

    auto function = utils::get_symbol<int(*)(std::shared_ptr<modules_interaction::TdWrap>)>(handle, alias_function);
    if (!function) {
        logger->log(spdlog::level::err,
                    "{}: It looks like the module does not contain a function with the signature '{}(std::shared_ptr<lit::modules_interaction::TdWrap>)': {}",
                    __PRETTY_FUNCTION__, module_info.name(), dlerror());
        return false;
    } else {
        std::thread run_module_safe([function, command, message, handle, args, module_info]() {
            utils::drop_privileges();

            try {
                auto get_response_wrapper = [](td::td_api::object_ptr<td::td_api::Function> req) { return td_api::get_response(std::move(req), runtime_storage::LITRequestId++); };
                function(std::make_shared<modules_interaction::TdWrap>(get_response_wrapper, std::move(command), args, message));
            } catch (std::exception& exc) {
                logger->log(spdlog::level::warn,
                            "{}: The '{}' threw the std::exception(or its derivatives): {}",
                            __PRETTY_FUNCTION__, module_info.name(), exc.what());
                std::terminate();
            } catch (...) {
                logger->log(spdlog::level::warn,
                            "{}: The '{}' threw a unknown exception",
                            __PRETTY_FUNCTION__, module_info.name());
                std::terminate();
            }
        });
        run_module_safe.join();
        dlclose(handle);
        logger->log(spdlog::level::debug,
                    "{}: The library object has been released",
                    __PRETTY_FUNCTION__);
        return true;
    }
}
