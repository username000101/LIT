#ifndef LIT_RUNTIMESTORAGE_RUNTIMESTORAGE_HXX
#define LIT_RUNTIMESTORAGE_RUNTIMESTORAGE_HXX

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <Configuration/LITConfiguration.hxx>
#include <Utils/Macros.hxx>
#include <ModulesInteraction/ModulesInteraction.hxx>
#include <Utils/MutexWrap.hxx>

namespace lit {
    namespace runtime_storage {
        inline std::shared_ptr<cfg::LITCfg> LITConfig;

        /* module_name:module_info */
        inline std::shared_ptr<td::ClientManager> LITClient;
        inline td::ClientManager::ClientId LITClientId;
        inline std::shared_ptr<std::unordered_map<std::string, modules_interaction::ModuleInfo>> LITModules;
        inline utils::MutWrap LITMutex;
        inline std::shared_ptr<std::queue<td::td_api::object_ptr<td::td_api::Function>>> LITTdQueriesQueue;
        inline LIT_EXPORT std::uint64_t LITRequestId = 1;

        namespace vars {
            inline std::string command_prefix = ".";
        }
    }
}

#endif // LIT_RUNTIMESTORAGE_RUNTIMESTORAGE_HXX

