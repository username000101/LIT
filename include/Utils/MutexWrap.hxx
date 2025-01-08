#ifndef LIT_UTILS_MUTEXWRAP_HXX
#define LIT_UTILS_MUTEXWRAP_HXX

#include <atomic>
#include <mutex>
#include <thread>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

namespace utils {
    class MutWrap {
    public:
        MutWrap() : is_available_(true) {}

        void use(const std::string& client_name) {
            static auto logger = spdlog::get("MutWrap");
            ASSERT(logger, "The 'MutWrap' logger is not initialized");

            if (this->is_available_) {
                logger->log(spdlog::level::debug,
                            "{}: Requested lock for '{}': OK",
                            __PRETTY_FUNCTION__, client_name);
                this->is_available_ = false;
                this->locked_by_ = client_name;
            } else {
                logger->log(spdlog::level::debug,
                            "{}: Requested lock for '{}': ERR: LOCKED(stuck in wait loop)",
                            __PRETTY_FUNCTION__, client_name);
                while (!this->is_available_) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }
                logger->log(spdlog::level::debug,
                            "{}: Request for lock has been satisfied for '{}'",
                            __PRETTY_FUNCTION__, client_name);
                return this->use(client_name);
            }
        }

        bool free(const std::string& client_name) {
            static auto logger = spdlog::get("MutWrap");
            ASSERT(logger, "The 'MutWrap' logger is not initialized");

            if (this->is_available_) {
                logger->log(spdlog::level::debug,
                            "{}: Requested unlock for '{}': ERR: NOT_LOCKED",
                            __PRETTY_FUNCTION__, client_name);
                return false;
            } else if (this->locked_by_ != client_name) {
                logger->log(spdlog::level::debug,
                            "{}: Requested unlock for '{}': ERR: INVALID_CLIENT_NAME(locked by '{}')",
                            __PRETTY_FUNCTION__, client_name, this->locked_by_);
                return false;
            } else {
                logger->log(spdlog::level::debug,
                            "{}: Requested unlock for '{}': OK",
                            __PRETTY_FUNCTION__, client_name);
                this->is_available_ = true;
                this->locked_by_.clear();
                return true;
            }
        }

        bool check() { return this->is_available_; }
    private:
        std::atomic<bool> is_available_;
        std::string locked_by_;
    };
}
#endif // LIT_UTILS_MUTEXWRAP_HXX
