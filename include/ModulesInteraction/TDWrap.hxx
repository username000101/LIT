#ifndef LIT_MODULESINTERACTION_TDWRAP_HXX
#define LIT_MODULESINTERACTION_TDWRAP_HXX

#include <td/telegram/Client.h>

namespace lit {
    namespace modules_interaction {
        /*
         * TdWrap is a method of abstracting the Stdlib client from the end module.
         * TdWrap provides wrappers for interacting with the Telegram API without providing direct access to the TDLib instance
         */
        class TdWrap {
        public:
            typedef td::ClientManager::Response(*update_handler)
                    (td::td_api::object_ptr<td::td_api::Function>, td::ClientManager::ClientId);

            TdWrap operator=(TdWrap& other) = delete;
            TdWrap(TdWrap& other) = delete;
            TdWrap(TdWrap&& other) = delete;
            TdWrap(update_handler update_function) : update_function_(update_function) {}

            [[nodiscard]] td::ClientManager::Response send_request
                   (td::td_api::object_ptr<td::td_api::Function> request) const noexcept
                   { return this->update_function_(std::move(request), this->td_request_id_++); }
        private:
            mutable td::ClientManager::RequestId td_request_id_;
            update_handler update_function_;
        };
    }
}

#endif // LIT_MODULESINTERACTION_TDWRAP_HXX
