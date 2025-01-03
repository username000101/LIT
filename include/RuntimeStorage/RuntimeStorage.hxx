#ifndef LIT_RUNTIMESTORAGE_RUNTIMESTORAGE_HXX
#define LIT_RUNTIMESTORAGE_RUNTIMESTORAGE_HXX

#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <Configuration/LITConfiguration.hxx>
#include <ModulesInteraction/ModulesInteraction.hxx>

namespace lit {
    namespace runtime_storage {
        inline std::shared_ptr<cfg::LITCfg> LITConfig;

        /* module_name:module_info */
        inline std::shared_ptr<td::ClientManager> LITClient;
        inline std::shared_ptr<std::unordered_map<std::filesystem::path, modules_interaction::ModuleInfo>> LITModules;
        inline std::shared_ptr<std::queue<td::td_api::object_ptr<td::td_api::Function>>> LITTdQueriesQueue;
        inline std::uint64_t LITRequestId = 0;

        inline static std::mutex queue_mtx;

        inline void add_in_query(td::td_api::object_ptr<td::td_api::Function> query) {
            queue_mtx.lock();
            LITTdQueriesQueue->push(std::move(query));
            queue_mtx.unlock();
        }

        inline td::td_api::object_ptr<td::td_api::Function> get_from_query() {
            queue_mtx.lock();
            auto buffer = std::move(LITTdQueriesQueue->front());
            LITTdQueriesQueue->pop();
            queue_mtx.unlock();
            return std::move(buffer);
        }
    }
}

#endif // LIT_RUNTIMESTORAGE_RUNTIMESTORAGE_HXX

