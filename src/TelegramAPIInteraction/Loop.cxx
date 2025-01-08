#include <TelegramAPIInteraction/Loop.hxx>

#include <functional>
#include <string>

#include <td/telegram/Client.h>
#include <spdlog/spdlog.h>

#include <Utils/Cast.hxx>
#include <XLML/ExecModule.hxx>
#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Auth/TDLibAuthentification.hxx>
#include <Utils/Macros.hxx>
#include <Utils/GetUpdate.hxx>
#include <TelegramAPIInteraction/Updates.hxx>

void lit::td_api::lit_loop() {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    using runtime_storage::LITClient;
    using runtime_storage::LITClientId;
    using runtime_storage::LITMutex;

    if (!LITClient) {
        LITClient = std::make_shared<td::ClientManager>();
        if (!LITClient) {
            logger->log(spdlog::level::critical,
                        "{}: Failed to create LITClient",
                        __PRETTY_FUNCTION__);
            std::exit(-1);
        }

        LITClient->execute(std::move(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0)));
        LITClientId = LITClient->create_client_id();
    }

    while (true) {
        auto current_authorization_state = td_api::get_response(std::move(td::td_api::make_object<td::td_api::getAuthorizationState>()));
        if (!current_authorization_state.object) {
            logger->log(spdlog::level::warn,
                        "{}: Returned invalid response, trying again...",
                        __PRETTY_FUNCTION__);
        } else {
            if (current_authorization_state.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(current_authorization_state.object);
                logger->log(spdlog::level::critical,
                            "{}: TelegramAPI returned error: {}",
                            __PRETTY_FUNCTION__, error->message_);
                LITClient.reset();
                std::exit(-2);
            } else {
                switch (current_authorization_state.object->get_id()) {
                    case td::td_api::authorizationStateWaitTdlibParameters::ID: {
                        td_auth::set_tdlibparameters();
                        continue;
                    }

                    case td::td_api::authorizationStateWaitPhoneNumber::ID: {
                        td_auth::auth_in_account();
                        continue;
                    }

                    case td::td_api::authorizationStateReady::ID: {
                        break;
                    }
                }
                break;
            }
        }
    }

    logger->log(spdlog::level::info,
                "{}: It looks like the LIT is ready to work, but this part is not implemented now, be careful",
                __PRETTY_FUNCTION__);

    logger->log(spdlog::level::info,
                "{}: The LIT v{} {} has been started!",
                __PRETTY_FUNCTION__, LIT_VERSION, LIT_VERSION_TYPE);
    while (true) {
        auto new_update = utils::get_td_updates();
        if (!new_update.object) {
            logger->log(spdlog::level::debug,
                        "{}: Invalid update received(perhaps there are no updates?), trying again",
                        __PRETTY_FUNCTION__);
            continue;
        } else {
            switch (new_update.object->get_id()) {
                case td::td_api::updateNewMessage::ID: {
                    auto message = utils::tl_uptr_cast<td::td_api::updateNewMessage>
                        (td::move_tl_object_as<td::td_api::updateNewMessage>(new_update.object));
                    if (!message->message_->is_outgoing_ || message->message_->content_->get_id() != td::td_api::messageText::ID) {
                        continue;
                    }
                    else {
                        auto message_content = td::move_tl_object_as<td::td_api::messageText>(message->message_->content_);
                        if (!message_content->text_->text_.starts_with(runtime_storage::vars::command_prefix)) {
                            logger->log(spdlog::level::debug,
                                        "{}: Non-command message",
                                        __PRETTY_FUNCTION__);
                            continue;
                        }
                        logger->log(spdlog::level::info,
                                    "{}: The outgoing message may be a command\ntext='{}'",
                                    __PRETTY_FUNCTION__, message_content->text_->text_);
                        auto message_ptr = utils::td_object_cast(std::move(message->message_));
                        std::thread async_module_start([message_content = std::move(message_content), message_ptr]() {
                            xlml::start_module(message_content->text_->text_.substr(runtime_storage::vars::command_prefix.length()), message_ptr);
                        });
                        async_module_start.detach();
                    }
                }
            }
        }
    }
}
