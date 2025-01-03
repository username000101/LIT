#ifndef LIT_AUTH_TDLIBAUTHENTIFICATION_HXX
#define LIT_AUTH_TDLIBAUTHENTIFICATION_HXX

#include <memory>
#include <string>

#include <td/telegram/Client.h>

namespace lit {
    namespace td_auth {
        bool auth_in_account(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id);
        bool set_tdlibparameters(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id);
    }
}

#endif // LIT_AUTH_TDLIBAUTHENTIFICATION_HXX
