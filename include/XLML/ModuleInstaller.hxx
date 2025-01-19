#ifndef LIT_XLML_ZMODULEINSALLER_HXX
#define LIT_XLML_ZMODULEINSALLER_HXX

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <Utils/Macros.hxx>

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

        class LIT_EXPORT ModuleInstaller {
        public:
            enum ModuleType { ARCHIVE, LIB, INVALID, };
            ModuleInstaller() {}

            bool operator()(const std::filesystem::path& module_path) {
                if (!std::filesystem::exists(module_path)) {
                    spdlog::log(spdlog::level::err,
                                "{}: Failed to install module {}: file not found",
                                __PRETTY_FUNCTION__, module_path.string());
                    return false;
                } else {
                    if (module_path.extension().string() == ".so")
                        return this->start_install_library(module_path);
                    else
                        return this->start_install_archive(module_path);
                }
            }
        private:
            nlohmann::json read_and_merge_module_config(const std::string& config, const std::string& modules_config,
                                                        const std::filesystem::path& module_path);

            bool start_install_archive(const std::filesystem::path& module); // For .zip/.7z/.tar.gz/.tar.xz/etc modules
            bool start_install_library(const std::filesystem::path& module); // For .so libraries
            bool start_install_library_exconf(const std::filesystem::path& module,
                                              const std::filesystem::path& config); // For .so libraries with an external config
        };
    }
}

#endif // LIT_XLML_ZMODULEINSALLER_HXX
