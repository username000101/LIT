#ifndef LIT_MODULESINTERACTION_TDWRAP_HXX
#define LIT_MODULESINTERACTION_TDWRAP_HXX

#include <cstdint>
#include <memory>
#include <iostream>

#include <td/telegram/Client.h>

namespace lit {
    namespace runtime_storage {
        extern std::uint64_t LITRequestId;
    }

    namespace modules_interaction {
        /*
         * TdWrap is a method of abstracting the tdlib client from the module.
         * TdWrap provides wrappers for interacting with the Telegram API without providing direct access to the TDLib instance
         */
        class __attribute__((visibility("default"))) TdWrap {
        public:
            using update_handler = td::ClientManager::Response(*)
                (td::td_api::object_ptr<td::td_api::Function>, td::ClientManager::RequestId);

            TdWrap operator=(TdWrap& other) = delete;
            TdWrap(TdWrap& other) = delete;
            TdWrap(TdWrap&& other) = delete;
            TdWrap(update_handler update_function, const std::string& call_command,
                   std::shared_ptr<td::td_api::message> message) :
                update_function_(update_function), call_command_(call_command),
                message_(message) {
                if (!message) {
                    std::cout << "FROM MODULE::TdWrap: THE message IS INVALID" << std::endl;
                    std::exit(-1);
                }
            }

            [[nodiscard]] td::ClientManager::Response send_request
                   (td::td_api::object_ptr<td::td_api::Function> request) const noexcept {
                        if (!this->update_function_) {
                            std::cout << "FROM MODULE::TdWrap: THE update_function_ IS NOT CALLABLE" << std::endl;
                        }
                        return this->update_function_(std::move(request), runtime_storage::LITRequestId++);
                   }

            [[nodiscard]] std::shared_ptr<td::td_api::message> get_message() const noexcept
                   { return this->message_; }

            [[nodiscard]] std::string get_call_command() const noexcept
                   { return this->call_command_; }
        private:
            update_handler update_function_;
            std::string call_command_;
            std::shared_ptr<td::td_api::message> message_;
        };
    }
}

#endif // LIT_MODULESINTERACTION_TDWRAP_HXX
