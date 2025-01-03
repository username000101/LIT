#include <Filesystem/EnvironmentSetup.hxx>

#include <chrono>
#include <filesystem>

#include <spdlog/spdlog.h>

#include <Utils/StringsBinder.hxx>
#include <Utils/Macros.hxx>

void lit::env::setup::setup_loggers(std::vector<std::pair<std::string, spdlog::level::level_enum>> loggers, const std::filesystem::path& PREFIX) {
    if (!std::filesystem::exists(PREFIX)) {
        spdlog::log(spdlog::level::critical, "{}: Setup loggers error: invalid PREFIX provided(prefix=\"{}\")", __FUNCTION__, PREFIX.string());
        std::abort();
    }

    spdlog::flush_every(std::chrono::milliseconds(1500));
    auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream formattedTime;
    formattedTime << std::put_time(std::localtime(&now_time_t), "___%F__%H__%M___");
    std::string log_file_name = vars::get_lit_logs_dir() + "/LIT_LOG" + formattedTime.str() + ".txt";

    for (auto& logger : loggers) {
        auto new_logger = std::make_shared<spdlog::logger>
        (logger.first, spdlog::sinks_init_list{ LIT_SPDLOG_MAKE_STDOUT_SINK(),
        LIT_SPDLOG_MAKE_FILE_SINK(log_file_name) });
        new_logger->set_level(logger.second);
        spdlog::register_logger(new_logger);
    }

    spdlog::set_default_logger(std::make_shared<spdlog::logger>
        ("DefaultLogger", spdlog::sinks_init_list{ LIT_SPDLOG_MAKE_STDOUT_SINK(),
         LIT_SPDLOG_MAKE_FILE_SINK(log_file_name)}));

    spdlog::log(spdlog::level::info, "{}: All loggers are initialized", __FUNCTION__);
}
