#include <TelegramAPIInteraction/Loop.hxx>

#include <string>
#include <sstream>

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
                        logger->log(spdlog::level::debug,
                                    "{}: Sending tdlib parameters...",
                                    __PRETTY_FUNCTION__);
                        td_auth::set_tdlibparameters();
                        continue;
                    }

                    case td::td_api::authorizationStateWaitPhoneNumber::ID: {
                        logger->log(spdlog::level::debug,
                                    "{}: Waiting for auth...",
                                    __PRETTY_FUNCTION__);
                        td_auth::auth_in_account();
                        continue;
                    }

                    case td::td_api::authorizationStateReady::ID: {
                        logger->log(spdlog::level::debug,
                                    "{}: Tdlib client is ready",
                                    __PRETTY_FUNCTION__);
                        break;
                    }

                    case td::td_api::updateConnectionState::ID: {
                        logger->log(spdlog::level::warn,
                                    "{}: The connection has been changed",
                                    __PRETTY_FUNCTION__);
                        continue;
                    }

                    default: {
                        logger->log(spdlog::level::debug,
                                    "{}: Unexpected response(object id: {})",
                                    __PRETTY_FUNCTION__, current_authorization_state.object->get_id());
                        continue;
                    }
                }
                break;
            }
        }
    }

    logger->log(spdlog::level::info,
                "{}: It looks like the LIT is ready to work, but this part is not fully implemented, be careful",
                __PRETTY_FUNCTION__);

    logger->log(spdlog::level::info,
                "{}: The LIT v{} {} has been started!",
                __PRETTY_FUNCTION__, LIT_VERSION, LIT_VERSION_TYPE);
    while (true) {
        auto new_update = utils::get_td_updates();
        if (!new_update.object) {
            logger->log(spdlog::level::debug,
                        "{}: Invalid update received(perhaps there are no updates?(requestId: {})), trying again",
                        __PRETTY_FUNCTION__, new_update.request_id);
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
                        logger->log(spdlog::level::debug,
                                    "{}: The outgoing message may be a command\ntext='{}'",
                                    __PRETTY_FUNCTION__, message_content->text_->text_);
                        auto message_ptr = utils::td_object_cast(std::move(message->message_));
                        auto message_text = message_content->text_->text_.substr(runtime_storage::vars::command_prefix.length());
                        std::istringstream rstream(message_text);
                        std::string command;
                        rstream >> command;
                        std::string buffer;
                        std::vector<std::string> args;
                        while (rstream >> buffer)
                            args.push_back(buffer);
                        std::thread async_module_start([message_ptr, args = std::move(args), message_text = std::move(command)] {
                            xlml::start_module(message_text, args, message_ptr);
                        });
                        async_module_start.detach();
                    }
                }
            }
        }
    }
}
