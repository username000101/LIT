#include <Auth/TDLibAuthentification.hxx>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>

#include <TelegramAPIInteraction/Updates.hxx>

bool lit::td_auth::set_tdlibparameters(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id) {
    if (!client) {
        spdlog::log(spdlog::level::err,
                    "{}: Failed to set tdlib parameters: invalid client ptr provided",
                    __FUNCTION__);
        return false;
    }

    td_api::get_response(std::move(td::td_api::setTdlibParameters(false, LIT_TDLIB_DATABASE_DIR, LIT_TDLIB_FILES_DIR,
                                                           "", true, true,
                                                           true, true, std::stoll(LIT_API_ID),
                                                           LIT_API_HASH, "ru_RU", "Linux device", "Unknown", LIT_VERSION)), );
}
