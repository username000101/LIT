#include <TelegramAPIInteraction/Updates.hxx>

#include <algorithm>
#include <unordered_map>

#include <td/telegram/Client.h>
#include <Utils/Macros.hxx>
#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/GetUpdate.hxx>
#include <TelegramAPIInteraction/Loop.hxx>

static std::unordered_map<td::ClientManager::RequestId, td::ClientManager::Response> unconfirmed_updates;

td::ClientManager::Response lit::td_api::get_response(td::td_api::object_ptr<td::td_api::Function> req, td::ClientManager::RequestId req_id) {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    using runtime_storage::LITClient;
    using runtime_storage::LITConfig;
    using runtime_storage::LITRequestId;
    using runtime_storage::LITClientId;
    using runtime_storage::REQMutex;

    auto blocked_requests = LITConfig->blocked_requests();


    if (req_id == 0)
        return {};
    else if (std::find(blocked_requests.begin(), blocked_requests.end(), req->get_id()) != blocked_requests.end()) {
        logger->log(spdlog::level::warn,
                    "{}: Attempt to send an invalid request({})",
                    __PRETTY_FUNCTION__, req->get_id());
        return {};
    }
    logger->log(spdlog::level::trace,
                "{}: Getting response for request_id {} and td::td_api::Function::ID {}",
                __PRETTY_FUNCTION__, req_id, req->get_id());

    if (unconfirmed_updates.count(req_id) > 0) {
        logger->log(spdlog::level::debug,
                    "{}: The response to the request with id {} was found locally",
                    __PRETTY_FUNCTION__, req_id);
        auto response_tmp = std::move(unconfirmed_updates.at(req_id));
        unconfirmed_updates.erase(req_id);
        return std::move(response_tmp);
    }

    LITClient->send(LITClientId, req_id, std::move(req));
    for (int attempts = 1; attempts < LIT_TDLIB_ATTEMPTS; ++attempts) {
        auto result = utils::get_td_updates();
        if (!result.object) {
            logger->log(spdlog::level::warn,
                "{}: Invalid response received(object == nullptr; request_id == {})",
                __PRETTY_FUNCTION__, result.request_id);
        } else if (result.request_id != req_id && result.request_id != 0) {
            logger->log(spdlog::level::trace,
                "{}: Received response with request_id == {} but expected {}",
                __PRETTY_FUNCTION__, result.request_id, req_id);
            unconfirmed_updates[result.request_id] = std::move(result);
        } else
            return result;
    }

    logger->log(spdlog::level::warn,
                "{}: Attempts ended, but no response was received for the request with request_id {}",
                __PRETTY_FUNCTION__, req_id);
    return {};
}
