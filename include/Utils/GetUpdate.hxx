#ifndef LIT_UTILS_GETUPDATE_HXX
#define LIT_UTILS_GETUPDATE_HXX

#include <td/telegram/Client.h>
#include <RuntimeStorage/RuntimeStorage.hxx>

namespace utils {
    [[nodiscard]] inline td::ClientManager::Response get_td_updates(double timeout = LIT_TDLIB_TIMEOUT) {
        using lit::runtime_storage::LITClient;
        using lit::runtime_storage::UPDMutex;

        std::lock_guard<yamc::fair::mutex> lock(UPDMutex);
        auto response = LITClient->receive(timeout);
        return response;
    }
}

#endif // LIT_UTILS_GETUPDATE_HXX
