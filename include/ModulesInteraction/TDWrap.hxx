#ifndef LIT_MODULESINTERACTION_TDWRAP_HXX
#define LIT_MODULESINTERACTION_TDWRAP_HXX

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <td/telegram/Client.h>

extern "C" std::tuple<int, int, int> TDWRAP_LAYER_VERSION = {1, 0, 0};

namespace lit {
    namespace modules_interaction {
        /*
         * TdWrap is a method of abstracting the tdlib client from the module.
         * TdWrap provides wrappers for interacting with the Telegram API without providing direct access to the TDLib instance
         */
        class __attribute__((visibility("default"))) TdWrap {
        public:
            using update_handler = td::ClientManager::Response(*)
                (td::td_api::object_ptr<td::td_api::Function>);

            TdWrap operator=(TdWrap& other) = delete;
            TdWrap(TdWrap& other) = delete;
            TdWrap(TdWrap&& other) = delete;
            TdWrap(update_handler update_function, std::string call_command,
                   std::vector<std::string> args, std::shared_ptr<td::td_api::message> message) :
                update_function_(update_function), call_command_(call_command),
                message_(message) {
                if (!message)
                    std::exit(-1);

                this->args_ = args;
            }

            [[nodiscard]] td::ClientManager::Response send_request
                   (td::td_api::object_ptr<td::td_api::Function> request) const noexcept {
                        if (!this->update_function_)
                            return {};
                        return this->update_function_(std::move(request));
                   }

            [[nodiscard]] std::shared_ptr<td::td_api::message> get_message() const noexcept {
                if (this->message_)
                    return this->message_;
                else
                    return nullptr;
            }

            [[nodiscard]] std::string get_call_command() const noexcept { return this->call_command_; }

            [[nodiscard]] std::vector<std::string> get_args() { return this->args_; }
        private:
            update_handler update_function_;
            std::vector<std::string> args_;
            std::string call_command_;
            std::shared_ptr<td::td_api::message> message_;
        };
    }
}

#endif // LIT_MODULESINTERACTION_TDWRAP_HXX
