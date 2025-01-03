#ifndef LIT_FILESYSTEM_ENVIRONMENTSETUP_HXX
#define LIT_FILESYSTEM_ENVIRONMENTSETUP_HXX

#include <filesystem>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

namespace lit {
    namespace env {
        namespace vars {
            [[nodiscard]] constexpr std::string get_lit_version() noexcept { return LIT_VERSION; }
            [[nodiscard]] constexpr std::string get_lit_dir() noexcept { return LIT_DIR; }
            [[nodiscard]] constexpr std::string get_lit_config() noexcept { return LIT_CONFIG; }
            [[nodiscard]] constexpr std::string get_lit_modules_config() noexcept { return LIT_MODULES_CONFIG; }
            [[nodiscard]] constexpr std::string get_lit_modules_dir() noexcept { return LIT_MODULES_DIR; }
            [[nodiscard]] constexpr std::string get_lit_files_dir() noexcept { return LIT_FILES_DIR; }
            [[nodiscard]] constexpr std::string get_lit_translations_dir() noexcept { return LIT_TRANSLATIONS_DIR; }
            [[nodiscard]] constexpr std::string get_lit_logs_dir() noexcept { return LIT_LOGS_DIR; }
        }

        namespace setup {
            void setup_environment(const std::filesystem::path& PREFIX = vars::get_lit_dir());
            void setup_loggers(std::vector<std::pair<std::string, spdlog::level::level_enum>> loggers, const std::filesystem::path& PREFIX = vars::get_lit_logs_dir());

            void clean_environment(const std::filesystem::path& PREFIX = vars::get_lit_dir());
        }
    }
}

#endif // LIT_FILESYSTEM_ENVIRONMENTSETUP_HXX
