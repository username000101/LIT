#ifndef LIT_UTILS_ACCESS_HXX
#define LIT_UTILS_ACCESS_HXX

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

namespace utils {
#if defined(unix)
#include <unistd.h>

    bool drop_privileges(uid_t new_uid = 1000, gid_t new_gid = 1000) {
        static auto logger = spdlog::get("LIT");
        ASSERT(logger, "The 'LIT' logger is not initialized");

        [[unlikely]] if (setgid(new_gid) != 0) {
            logger->log(spdlog::level::err,
                        "{}: Failed to drop privileges",
                        __PRETTY_FUNCTION__);
            return false;
        }

        [[unlikely]] if (setuid(new_uid) != 0) {
            logger->log(spdlog::level::err,
                        "{}: Failed to drop privileges",
                        __PRETTY_FUNCTION__);
            return false;
        }

        return true;
    }
#else
#pragma message("Not implemented for Windows")
    bool drop_privileges(int stub1 = 0, int stub2 = 0) {
        static auto logger = spdlog::get("LIT");
        ASSERT(logger, "The 'LIT' logger is not initialized");

        logger->log(spdlog::level::debug,
            "{}: The access drop is not implemented for Windows",
            __PRETTY_FUNCTION__);
        return true;
    }
#endif
}
#endif // LIT_UTILS_ACCESS_HXX
