#include <TelegramAPIInteraction/Updates.hxx>

#include <mutex>

#include <td/telegram/Client.h>
#include <Utils/Macros.hxx>
#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
#include <TelegramAPIInteraction/Loop.hxx>

std::mutex gl_mutex;


td::ClientManager::Response lit::td_api::get_response(td::td_api::object_ptr<td::td_api::Function> req, td::ClientManager::RequestId req_id) {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    using runtime_storage::LITClient;
    using runtime_storage::LITConfig;
    using runtime_storage::LITRequestId;
    static td::ClientManager::ClientId client_id = LITClient->create_client_id();

    if (req_id == 0)
        return {};
    gl_mutex.lock();

    LITClient->send(client_id, req_id, std::move(req));
    for (int attempts = 1; attempts < LIT_TDLIB_ATTEMPTS; ++attempts) {
        auto result = LITClient->receive(LIT_TDLIB_TIMEOUT);
        if (!result.object) {
            logger->log(spdlog::level::warn,
                "{}: Invalid response received(object == nullptr; request_id == {})",
                __FUNCTION__, result.request_id);
        } else if (result.request_id != req_id) {
            logger->log(spdlog::level::trace,
                "{}: Received response with request_id == {} but expected {}",
                __FUNCTION__, result.request_id, req_id);
        } else {
            gl_mutex.unlock();
            return result;
        }
    }

    gl_mutex.unlock();
    return {};
}
