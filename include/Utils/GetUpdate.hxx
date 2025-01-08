#ifndef LIT_UTILS_GETUPDATE_HXX
#define LIT_UTILS_GETUPDATE_HXX

#include <td/telegram/Client.h>

#include <RuntimeStorage/RuntimeStorage.hxx>

namespace utils {
    [[nodiscard]] inline td::ClientManager::Response get_td_updates(double timeout = LIT_TDLIB_TIMEOUT) {
        using lit::runtime_storage::LITClient;
        using lit::runtime_storage::LITMutex;

        LITMutex.use("get_td_updates");
        auto response = LITClient->receive(timeout);
        LITMutex.free("get_td_updates");
        return response;
    }
}

#endif // LIT_UTILS_GETUPDATE_HXX
