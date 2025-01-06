#ifndef LIT_UTILS_ACCESS_HXX
#define LIT_UTILS_ACCESS_HXX

#include <unistd.h>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

namespace utils {
    bool drop_privileges(uid_t new_uid = 1000, gid_t new_gid = 1000) {
        static auto logger = spdlog::get("LIT");
        ASSERT(logger, "The 'LIT' logger is not initialized");

        if (setgid(new_gid) != 0) {
            logger->log(spdlog::level::err,
                        "{}: Failed to drop privileges",
                        __FUNCTION__);
            return false;
        }

        if (setuid(new_uid) != 0) {
            logger->log(spdlog::level::err,
                        "{}: Failed to drop privileges",
                        __FUNCTION__);
            return false;
        }

        return true;
    }
}
#endif // LIT_UTILS_ACCESS_HXX
