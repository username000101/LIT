// ===> LIT: Limber Interactive Tool <===
//       (or Lame Incompetent Think?)

#include <csignal>

#include <Args/CLArgs.hxx>
#include <Configuration/LITConfiguration.hxx>
#include <Filesystem/EnvironmentSetup.hxx>
#include <RuntimeStorage/RuntimeStorage.hxx>
#include <Utils/Saver.hxx>
#include <XLML/ModuleInstaller.hxx>
#include <TelegramAPIInteraction/Loop.hxx>
#include <XLML/ModulesConfigurationReader.hxx>
#include <Utils/TerminalIO.hxx>

int main(int argc, char** argv) {
    if (argc > 1)
        lit::args::process_args(argc, argv);

    lit::env::setup::setup_environment();
    lit::env::setup::setup_loggers(
        {
            {"ModuleLoader", spdlog::level::debug},
            {"ConfigurationManager", spdlog::level::debug},
            {"LIT", spdlog::level::info},
            {"XLML", spdlog::level::info},
            {"MutWrap", spdlog::level::info},
           }
        );

    std::signal(SIGABRT, utils::sighandler);
    std::signal(SIGINT, utils::sighandler);

    lit::runtime_storage::LITConfig = std::make_shared<lit::cfg::LITCfg>(lit::cfg::load_config());
    spdlog::log(spdlog::level::info,
                "{}: The main configuration has been successfully loaded ;)\nversion=\"{}\"\ndir=\"{}\"",
                __PRETTY_FUNCTION__, lit::runtime_storage::LITConfig->version(), lit::runtime_storage::LITConfig->dir());
    lit::runtime_storage::LITModules = std::make_shared<std::unordered_map<std::string, lit::modules_interaction::ModuleInfo>
                                                       >(lit::xlml::read_modules_configuration());

    lit::td_api::lit_loop();
    lit::env::setup::clean_environment();
}
