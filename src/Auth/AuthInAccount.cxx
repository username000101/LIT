#include <Auth/TDLibAuthentification.hxx>

#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/Macros.hxx>
#include <Utils/StringsBinder.hxx>
#include <Utils/TerminalIO.hxx>
#include <TelegramAPIInteraction/Updates.hxx>

#if defined(WIN32)
#define clear_console std::system("cls");
#else
#define clear_console std::system("clear");
#endif

#define auth_state td::td_api::make_object<td::td_api::getAuthorizationState>()

bool lit::td_auth::auth_in_account(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id) {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    using utils::terminal_io::bgColor;
    using utils::terminal_io::fgColor;
    using runtime_storage::LITRequestId;

    bool has_password = false, first_auth_state = false, phone_number_state = false, 
        second_auth_state = false, confirmation_code_state = false, 
        three_auth_state = false, password_state = false;
    std::string phone_number_gl = "", configrmation_code_gl = "", password_gl = "", password_hint = "";
    int attempts = 20;

    for (int i = 0; i < attempts; ++i) {
        auto first_auth_state_check = td_api::get_response(auth_state);
        if (!first_auth_state_check.object) {
            logger->log(spdlog::level::warn, 
                        "{}: first_auth_state_check: object is null",
                        __PRETTY_FUNCTION__);
            continue;
        } else {
            if (first_auth_state_check.object->get_id() == td::td_api::authorizationStateReady::ID) {
               logger->log(spdlog::level::info,
                           "{}: You are already logged in",
                           __PRETTY_FUNCTION__);
               return true;
            } else if (first_auth_state_check.object->get_id() == td::td_api::error::ID) {
               auto error = td::move_tl_object_as<td::td_api::error>(first_auth_state_check.object);
               logger->log(spdlog::level::err,
                           "{}: first_auth_state_check: TelegramAPI error: {}",
                           __PRETTY_FUNCTION__,
                           error->message_);
               continue;
            } else if (first_auth_state_check.object->get_id() == td::td_api::updateConnectionState::ID) {
               auto connection_info = td::move_tl_object_as<td::td_api::updateConnectionState>(first_auth_state_check.object);
               logger->log(spdlog::level::warn,
                           "{}: first_auth_state_check: update connection state: {}",
                           __PRETTY_FUNCTION__,
                           connection_info->state_->get_id());
               continue;
            } else if (first_auth_state_check.object->get_id() == td::td_api::authorizationStateWaitPhoneNumber::ID) {
               logger->log(spdlog::level::debug, 
                          "{}: update id is authorizationStateWaitPhoneNumber",
                          __PRETTY_FUNCTION__);
               first_auth_state = true;
               break;
            } else {
                logger->log(spdlog::level::warn,
                            "{}: first_auth_state_check: unknown update id: {}",
                            __PRETTY_FUNCTION__,
               first_auth_state_check.object->get_id());
               continue;
            }
        }
    }

    if (!first_auth_state)
        return false;

    while (true) {
        auto phone_number = utils::terminal_io::console_input(utils::bindStrings(fgColor["blue"], "Write your phone number>", fgColor["reset"]));
        if (phone_number.empty())
            continue;

        if (utils::terminal_io::console_choice(utils::bindStrings("Your phone number is '", phone_number, "', sure?[y/n]"), { "y", "n" }) == "y") {
            phone_number_gl = phone_number;
            break;
        } else
            continue;
    }

    for (int i = 0; i < attempts; ++i) {
        auto set_auth_phone = td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>();
        set_auth_phone->phone_number_ = phone_number_gl;
        auto phone_number_result = td_api::get_response(std::move(set_auth_phone));
        if (!phone_number_result.object) {
            logger->log(spdlog::level::warn,
                        "{}: phone_number: object is null",
                        __PRETTY_FUNCTION__);
            continue;
        }
        else {
            if (phone_number_result.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(phone_number_result.object);
                logger->log(spdlog::level::err,
                    "{}: phone_number: TelegramAPI error: {}",
                    __PRETTY_FUNCTION__,
                    error->message_);
            }
            else if (phone_number_result.object->get_id() == td::td_api::ok::ID) {
                phone_number_state = true;
                break;
            }
            else if (phone_number_result.object->get_id() == td::td_api::updateAuthorizationState::ID) {
                auto state = std::move(td::move_tl_object_as<td::td_api::updateAuthorizationState>(phone_number_result.object)->authorization_state_);
                if (state->get_id() == td::td_api::authorizationStateWaitCode::ID) {
                    phone_number_state = true;
                    break;
                } else {
                    logger->log(spdlog::level::warn,
                                "{}: phone_number: unexpected authorization state: {}",
                                __PRETTY_FUNCTION__,
                                state->get_id());
                    continue;
                }
            } else {
                logger->log(spdlog::level::warn,
                            "{}: phone_number: unknown update id: {}",
                            __PRETTY_FUNCTION__,
                            phone_number_result.object->get_id());
                continue;
            }
        }
    }

    if (!phone_number_state)
        return false;

    for (int i = 0; i < attempts; ++i) {
        auto second_auth_state_check = td_api::get_response(auth_state);
        if (!second_auth_state_check.object) {
            logger->log(spdlog::level::warn,
                "{}: second_auth_state_check: object is null",
                __PRETTY_FUNCTION__);
            continue;
        } else {
            if (second_auth_state_check.object->get_id() == td::td_api::authorizationStateWaitCode::ID) {
                second_auth_state = true;
                break;
            } else if (second_auth_state_check.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(second_auth_state_check.object);
                logger->log(spdlog::level::err,
                    "{}: second_auth_state_check: TelegramAPI error: {}",
                    __PRETTY_FUNCTION__,
                    error->message_);
                continue;
            } else {
                logger->log(spdlog::level::warn,
                    "{}: second_auth_state_check: unknown update id: {}",
                    __PRETTY_FUNCTION__,
                    second_auth_state_check.object->get_id());
                continue;
            }
        }
    }

    if (!second_auth_state)
        return false;

    while (true) {
        auto confirmation_code = utils::terminal_io::console_input(utils::bindStrings(fgColor["blue"], "Write confirmation code>", fgColor["reset"]));
        if (confirmation_code.empty())
            continue;

        if (utils::terminal_io::console_choice(utils::bindStrings("Confirmation code is '", confirmation_code, "', sure?[y/n]"), { "y", "n" }) == "y") {
            configrmation_code_gl = confirmation_code;
            break;
        } else
            continue;
    }

    for (int i = 0; i < attempts; ++i) {
        auto confirmation_code_check = td_api::get_response(td::td_api::make_object<td::td_api::checkAuthenticationCode>(configrmation_code_gl));
        if (!confirmation_code_check.object) {
            logger->log(spdlog::level::warn,
                "{}: confirmation_code_check: object is null",
                __PRETTY_FUNCTION__);
            continue;
        } else {
            if (confirmation_code_check.object->get_id() == td::td_api::authorizationStateWaitCode::ID) {
                confirmation_code_state = true;
                break;
            } else if (confirmation_code_check.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(confirmation_code_check.object);
                logger->log(spdlog::level::err,
                    "{}: confirmation_code_check: TelegramAPI error: {}",
                    __PRETTY_FUNCTION__,
                    error->message_);
                continue;
            } else {
                logger->log(spdlog::level::warn,
                    "{}: confirmation_code_check: unknown update id: {}",
                    __PRETTY_FUNCTION__,
                    confirmation_code_check.object->get_id());
                continue;
            }
        }
    }

    if (!confirmation_code_state)
        return false;

    for (int i = 0; i < attempts; ++i) {
        auto three_auth_state_check = td_api::get_response(auth_state);
        if (!three_auth_state_check.object) {
            logger->log(spdlog::level::warn,
                "{}: three_auth_state_check: object is null",
                __PRETTY_FUNCTION__);
            continue;
        } else {
            if (three_auth_state_check.object->get_id() == td::td_api::authorizationStateReady::ID) {
                logger->log(spdlog::level::info,
                            "{}: You have successfully logged in",
                            __PRETTY_FUNCTION__);
                break;
            }
            else if (three_auth_state_check.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(three_auth_state_check.object);
                logger->log(spdlog::level::err,
                    "{}: three_auth_state_check: TelegramAPI error: {}",
                    __PRETTY_FUNCTION__,
                    error->message_);
                continue;
            }
            else if (three_auth_state_check.object->get_id() == td::td_api::authorizationStateWaitPassword::ID) {
                password_hint = td::move_tl_object_as<td::td_api::authorizationStateWaitPassword>(three_auth_state_check.object)->password_hint_;
                has_password = true;
                three_auth_state = true;
                break;
            } else {
                logger->log(spdlog::level::warn,
                    "{}: three_auth_state_check: unknown update id: {}",
                    __PRETTY_FUNCTION__,
                    three_auth_state_check.object->get_id());
                continue;
            }
        }
    }

    if (!three_auth_state)
        return false;
    else
        if (!has_password)
            return true;

    while (true) {
        auto password = utils::terminal_io::console_input(utils::bindStrings(fgColor["blue"], "Your account is protected by a cloud password, enter the password", (password_hint.empty() ? std::string("") : std::string("(hint: \"" + password_hint + "\")")), ")>", fgColor["reset"]));
        if (password.empty())
            continue;

        if (utils::terminal_io::console_choice(utils::bindStrings("Password is '", password, "', sure?[y/n]"), { "y", "n" }) == "y") {
            password_gl = password;
            break;
        } else
            continue;
    }

    for (int i = 0; i < attempts; ++i) {
        auto password_check = td_api::get_response(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password_gl));
        if (!password_check.object) {
            logger->log(spdlog::level::warn,
                "{}: password_check: object is null",
                __PRETTY_FUNCTION__);
            continue;
        }
        else {
            if (password_check.object->get_id() == td::td_api::ok::ID) {
                password_state = true;
                break;
            }
            else if (password_check.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(password_check.object);
                logger->log(spdlog::level::err,
                    "{}: password_check: TelegramAPI error: {}",
                    __PRETTY_FUNCTION__,
                    error->message_);
                continue;
            }
            else {
                logger->log(spdlog::level::warn,
                    "{}: password_check: unknown update id: {}",
                    __PRETTY_FUNCTION__,
                    password_check.object->get_id());
                continue;
            }
        }
    }

    if (!password_state)
        return false;
    else
        return true;
}
