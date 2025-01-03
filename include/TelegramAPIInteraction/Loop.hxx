#ifndef LIT_TELEGRAMAPIINTERACTION_LOOP_HXX
#define LIT_TELEGRAMAPIINTERACTION_LOOP_HXX

#include <memory>
#include <mutex>

#include <td/telegram/Client.h>

namespace lit {
    namespace td_api {
        inline std::shared_ptr<td::ClientManager> gl_client;
        void lit_loop();
    }
}

#endif // LIT_TELEGRAMAPIINTERACTION_LOOP_HXX
