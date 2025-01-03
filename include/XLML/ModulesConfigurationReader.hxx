#ifndef LIT_XLML_MODULESONFIGURATIONREADER_HXX
#define LIT_XLML_MODULESONFIGURATIONREADER_HXX

#include <filesystem>
#include <unordered_map>

#include <Filesystem/EnvironmentSetup.hxx>
#include <ModulesInteraction/ModulesInteraction.hxx>

namespace lit {
    namespace xlml {
        std::unordered_map<std::filesystem::path, modules_interaction::ModuleInfo> read_modules_configuration(
                    const std::filesystem::path& modules_config_file = lit::env::vars::get_lit_modules_config());
    }
}

#endif // LIT_XLML_MODULESONFIGURATIONREADER_HXX
