#include <Auth/TDLibAuthentification.hxx>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <spdlog/spdlog.h>

#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/Macros.hxx>
#include <TelegramAPIInteraction/Updates.hxx>

bool lit::td_auth::set_tdlibparameters(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id, bool test_dc) {
    static auto logger = spdlog::get("LIT");
    ASSERT(logger, "The 'LIT' logger is not initialized");

    if (!client) {
        logger->log(spdlog::level::err,
                    "{}: Failed to set tdlib parameters: invalid client ptr provided",
                    __PRETTY_FUNCTION__);
        return false;
    }

    if (test_dc)
        logger->log(spdlog::level::warn,
                    "{}: Using test_dc, USE IT ONLY FOR TESTING",
                    __PRETTY_FUNCTION__);

    logger->log(spdlog::level::debug,
                "{}: Setting tdlib settings, api_id: {}",
                __PRETTY_FUNCTION__,
                static_cast<td::td_api::int32>(std::stol(LIT_API_ID)));

    auto response = td_api::get_response(std::move(td::td_api::make_object<td::td_api::setTdlibParameters>(test_dc, LIT_TDLIB_DATABASE_DIR, LIT_TDLIB_FILES_DIR,
                                   "", true, true,
                                   true, true, static_cast<td::td_api::int32>(std::stol(LIT_API_ID)),
                                   LIT_API_HASH, "en_EN", "Linux",
                                   "Unknown", LIT_VERSION)));
    if (response.object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(response.object);
        logger->log(spdlog::level::err,
                    "{}: Failed to set tdlib parameters: TelegramAPI returned error: {}",
                    __PRETTY_FUNCTION__, 
                    error->message_);
        return false;
    } else {
        logger->log(spdlog::level::info,
                    "{}: Ok",
                    __PRETTY_FUNCTION__);
        return true;
    }
}
