#ifndef LIT_TELEGRAMAPIINTERACTION_UPDATES_HXX
#define LIT_TELEGRAMAPIINTERACTION_UPDATES_HXX

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <RuntimeStorage/RuntimeStorage.hxx>

namespace lit {
    namespace td_api {
        td::ClientManager::Response get_response
            (td::td_api::object_ptr<td::td_api::Function> req,
             td::ClientManager::RequestId req_id = runtime_storage::LITRequestId++);
    }
}
#endif // LIT_TELEGRAMAPIINTERACTION_UPDATES_HXX
