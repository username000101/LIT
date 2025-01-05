#include <TelegramAPIInteraction/Loop.hxx>

#include <string>

#include <td/telegram/Client.h>
#include <spdlog/spdlog.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/Macros.hxx>
#include <TelegramAPIInteraction/Updates.hxx>

void lit::td_api::lit_loop() {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    using runtime_storage::LITClient;
    using runtime_storage::LITRequestId;

    LITClient = std::make_shared<td::ClientManager>();
    if (!LITClient) {
        logger->log(spdlog::level::critical,
                    "{}: Loop failed: LITClient is not created(maybe std::make_shared returns nullptr)",
                    __FUNCTION__);
    }

    LITClient->execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0));

    auto response = get_response(std::move(td::td_api::make_object<td::td_api::getAuthorizationState>()), 1);
    if (response.object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(response.object);
        logger->log(spdlog::level::critical,
                    "{}: TelegramAPI returned error(for getAuthorizationState): {}",
                    __FUNCTION__, error->message_);
        std::abort();
    } else {
        switch (response.object->get_id()) {
            case td::td_api::authorizationStateReady::ID: {
                logger->log(spdlog::level::info,
                            "{}: The TDLib is ready",
                            __FUNCTION__);
                break;
            }

            case td::td_api::authorizationStateWaitTdlibParameters::ID: {
                logger->log(spdlog::level::warn, "The TDLib is waiting parameters");
                break;
            }
        }
    }
}
