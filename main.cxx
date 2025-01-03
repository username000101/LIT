// ===> LIT: Limber Interactive Tool <===
//       (or Lame Incompetent Think?)

#include <Args/CLArgs.hxx>
#include <Configuration/LITConfiguration.hxx>
#include <Filesystem/EnvironmentSetup.hxx>
#include <RuntimeStorage/RuntimeStorage.hxx>
#include <TelegramAPIInteraction/Loop.hxx>
#include <XLML/ModulesConfigurationReader.hxx>

#include <XLML/Archive.hxx>

int main(int argc, char** argv) {
    if (argc > 1)
        lit::args::process_args(argc, argv);

    lit::env::setup::setup_environment();
    lit::env::setup::setup_loggers(
        {
            {"ModuleLoader", spdlog::level::info},
            {"ConfigurationManager", spdlog::level::info},
            {"LIT", spdlog::level::info},
            {"XLML", spdlog::level::info},
           }
        );

    lit::runtime_storage::LITConfig = std::make_shared<lit::cfg::LITCfg>(lit::cfg::load_config());
    spdlog::log(spdlog::level::info,
                "{}: The main configuration has been successfully loaded ;)\nversion=\"{}\"\ndir=\"{}\"",
                __FUNCTION__, lit::runtime_storage::LITConfig->version(), lit::runtime_storage::LITConfig->dir());
    lit::runtime_storage::LITModules = std::make_shared<
            std::unordered_map<std::filesystem::path, lit::modules_interaction::ModuleInfo>
        >(lit::xlml::read_modules_configuration());

    lit::td_api::lit_loop();

    lit::env::setup::clean_environment();
}
