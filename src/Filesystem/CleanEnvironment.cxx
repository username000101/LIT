#include <Filesystem/EnvironmentSetup.hxx>

#include <filesystem>
#include <vector>


void lit::env::setup::clean_environment(const std::filesystem::path& PREFIX) {
    std::vector<std::string> possibly_used_dirs = {
        "/tmp/lit",
    };

    for (auto& dir : possibly_used_dirs) {
        if (std::filesystem::exists(dir))
            std::filesystem::remove_all(dir);
    }
    spdlog::log(spdlog::level::info,
        "{}: The environment was cleaned succefully",
        __FUNCTION__);
}
