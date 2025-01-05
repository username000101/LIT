#include <Auth/TDLibAuthentification.hxx>

#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/Macros.hxx>
#include <Utils/StringsBinder.hxx>
#include <Utils/TerminalIO.hxx>
#include <TelegramAPIInteraction/Updates.hxx>

bool lit::td_auth::auth_in_account(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id) {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    using utils::terminal_io::bgColor;
    using utils::terminal_io::fgColor;

    std::system("clear");

    bool have_password = false;
    std::string phone_number, code, password, password_hint;
    while (true) {
        phone_number = utils::terminal_io::console_input
        (utils::bindStrings(fgColor["blue"], "Write your phone>", fgColor["reset"]));
        if (phone_number.empty()) {
            std::cout << fgColor["red"] << "Incorrect input, please try again" << fgColor["reset"] << std::endl;
            continue;
        } else {
            if (utils::terminal_io::console_choice(utils::bindStrings("Your number is '", phone_number, "', sure?"), {"y", "n"}) == "y")
                break;
            else
                continue;
        }
    }

    for (int attempt = 1; attempt <= 5; ++attempt) {
        auto phone_number_result = td_api::get_response(std::move(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phone_number, nullptr)));
        if (!phone_number_result.object) {
            logger->log(spdlog::level::warn,
                        "{}: td_api::get_response() returned invalid response; trying again({} attempts left)",
                        __FUNCTION__, 5 - attempt);
            continue;
        } else if (attempt == 5) {
            logger->log(spdlog::level::critical,
                        "{}: Attempts ended, aborting",
                        __FUNCTION__);
            runtime_storage::LITClient.reset();
            std::exit(-1);
        } else {
            if (phone_number_result.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(phone_number_result.object);
                logger->log(spdlog::level::warn,
                            "{}: td_api::get_response() returned error: '{}', trying again({} attempts left)",
                            __FUNCTION__, error->message_, 5 - attempt);
            } else
                break;
        }
    }

    while (true) {
        code = utils::terminal_io::console_input
            (utils::bindStrings(fgColor["blue"], "Enter the code that you should have received>", fgColor["reset"]));
        if (code.empty()) {
            std::cout << fgColor["red"] << "Incorrect input, please try again" << fgColor["reset"] << std::endl;
            continue;
        } else {
            if (utils::terminal_io::console_choice(utils::bindStrings("Your code is '", code, "', sure?"), {"y", "n"}) == "y")
                break;
            else
                continue;
        }
    }

    for (int attempt = 1; attempt <= 5; ++attempt) {
        auto code_result = td_api::get_response(std::move(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code)));
        if (!code_result.object) {
            logger->log(spdlog::level::warn,
                        "{}: td_api::get_response() returned invalid response; trying again({} attempts left)",
                        __FUNCTION__, 5 - attempt);
            continue;
        } else if (attempt == 5) {
            logger->log(spdlog::level::critical,
                        "{}: Attempts ended, aborting",
                        __FUNCTION__);
            runtime_storage::LITClient.reset();
            std::exit(-1);
        } else {
            if (code_result.object->get_id() == td::td_api::error::ID) {
                auto error = td::move_tl_object_as<td::td_api::error>(code_result.object);
                logger->log(spdlog::level::warn,
                            "{}: td_api::get_response() returned error: '{}', trying again({} attempts left)",
                            __FUNCTION__, error->message_, 5 - attempt);
            } else
                break;
        }
    }

    for (int attempt = 1; attempt <= 5; ++attempt) {
        auto current_authorization_state = td_api::get_response(std::move(td::td_api::make_object<td::td_api::getAuthorizationState>()));
        if (!current_authorization_state.object) {
            logger->log(spdlog::level::warn,
                        "{}: td_api::get_response() returned invalid response; trying again({} attempts left)",
                        __FUNCTION__, 5 - attempt);
            continue;
        } else if (attempt == 5) {
            logger->log(spdlog::level::critical,
                        "{}: Attempts ended, aborting",
                        __FUNCTION__);
            runtime_storage::LITClient.reset();
            std::exit(-1);
        } else {
            switch (current_authorization_state.object->get_id()) {
                case td::td_api::authorizationStateWaitPassword::ID: {
                    password_hint = td::move_tl_object_as<td::td_api::authorizationStateWaitPassword>(current_authorization_state.object)->password_hint_;
                    have_password = true;
                    break;
                }

                case td::td_api::authorizationStateReady::ID: {
                    have_password = false;
                    break;
                }
            }
            break;
        }
    }

    if (have_password) {
        while (true) {
            password = utils::terminal_io::console_input
                (utils::bindStrings(fgColor["blue"], "Enter the password (hint: '", password_hint, "')>", fgColor["reset"]));
            if (password.empty()) {
                std::cout << fgColor["red"] << "Incorrect input, please try again" << fgColor["reset"] << std::endl;
                continue;
            } else {
                if (utils::terminal_io::console_choice(utils::bindStrings("Your password is '", password, "', sure?"), {"y", "n"}) == "y")
                    break;
                else
                    continue;
            }
        }

        for (int attempt = 1; attempt < 5; ++attempt) {
            auto password_result = td_api::get_response(std::move(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password)));

        }
    } else
        return true;

}
