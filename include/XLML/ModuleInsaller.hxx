#ifndef LIT_XLML_ZMODULEINSALLER_HXX
#define LIT_XLML_ZMODULEINSALLER_HXX

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

namespace lit {
    namespace xlml {
        struct ModuleBuildConfig {
            std::vector<std::filesystem::path> source_files; // Can be empty if the module use CMake or another build system
            std::filesystem::path output_dir;
        };

        struct ModuleMainConfig {
            std::string name;
            std::string version;
            std::string author;
            std::filesystem::path functions_list;
            std::filesystem::path build_config; // For modules with local building
        };

        struct ModuleFullConfig {
            ModuleFullConfig(std::shared_ptr<ModuleMainConfig> mmconf, std::shared_ptr<ModuleBuildConfig> mbconf)
            : module_main_configuration_(mmconf), module_build_configuration_(mbconf) {

            }

            std::shared_ptr<ModuleMainConfig> get_module_main_config() { return this->module_main_configuration_; };
            std::shared_ptr<ModuleBuildConfig> get_module_build_config() { return this->module_build_configuration_; };
        private:
            std::shared_ptr<ModuleMainConfig> module_main_configuration_;
            std::shared_ptr<ModuleBuildConfig> module_build_configuration_;
        };

        class ModuleInstaller {
        public:
            enum ModuleType { ARCHIVE, LIB, INVALID, };
            ModuleInstaller(const std::filesystem::path& module) {
                if (!std::filesystem::exists(module)) {
                    spdlog::log(spdlog::level::err,
                        "{}: Failed to create ModuleInstaller object: the module '{}' doesn't exists",
                        __FUNCTION__, module.string());
                    this->module_type_ = INVALID;
                } else {
                    if (module.extension() == ".so")
                        this->module_type_ = LIB;
                    else
                        this->module_type_ = ARCHIVE;
                }
            }
        private:
            std::filesystem::path module_path_;
            ModuleType module_type_;

            void start_install_archive(); // For .zip/.7z/.tar.gz/.tar.xz/etc modules
            void start_install_library(); // For .so libraries
        };
    }
}

#endif // LIT_XLML_ZMODULEINSALLER_HXX
