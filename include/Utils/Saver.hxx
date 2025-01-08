#ifndef LIT_UTILS_SAVER_HXX
#define LIT_UTILS_SAVER_HXX

#include <csignal>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>
#include <Filesystem/EnvironmentSetup.hxx>
#include <TelegramAPIInteraction/Loop.hxx>

namespace utils {
    inline void sighandler(int signal) {
        static auto logger = spdlog::get("LIT");
        ASSERT(logger, "The 'LIT logger is not initialized");

        logger->log(spdlog::level::critical,
                    "{}: Called the sighandler, LIT has received signal {}, we are trying to clear the environment and start over...",
                    __PRETTY_FUNCTION__, signal);

        switch (signal) {
            case SIGABRT: {
                lit::env::setup::clean_environment();
                lit::env::setup::setup_environment();
                lit::td_api::lit_loop();
                break;
            }

            case SIGINT: {
                logger->log(spdlog::level::info,
                            "{}: The SIGINT, LIT signal will not be restarted",
                            __PRETTY_FUNCTION__);
                lit::env::setup::clean_environment();
                std::exit(0);
            }
        }
    }
}

#endif // LIT_UTILS_SAVER_HXX
