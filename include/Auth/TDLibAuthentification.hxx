#ifndef LIT_AUTH_TDLIBAUTHENTIFICATION_HXX
#define LIT_AUTH_TDLIBAUTHENTIFICATION_HXX

#include <memory>
#include <string>

#include <td/telegram/Client.h>

#include <RuntimeStorage/RuntimeStorage.hxx>

namespace lit {
    namespace td_auth {
        bool auth_in_account(std::shared_ptr<td::ClientManager> client = runtime_storage::LITClient,
                         td::ClientManager::ClientId client_id = runtime_storage::LITClientId);

        bool set_tdlibparameters(std::shared_ptr<td::ClientManager> client = runtime_storage::LITClient,
                                 td::ClientManager::ClientId client_id = runtime_storage::LITClientId, bool test_dc = false);
    }
}

#endif // LIT_AUTH_TDLIBAUTHENTIFICATION_HXX
