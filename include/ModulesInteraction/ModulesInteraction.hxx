#ifndef LIT_MODULESINTERACTION_MODULESINTERACTION_HXX
#define LIT_MODULESINTERACTION_MODULESINTERACTION_HXX

#include <dlfcn.h>

#include <csignal>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

namespace lit {
    namespace modules_interaction {
        class ModuleInfo {
        public:
            ModuleInfo() = default;
            ModuleInfo& operator=(const ModuleInfo& other) = default;
            ModuleInfo(const ModuleInfo& other) = default;

            ModuleInfo(ModuleInfo&& other) {
                this->version_ = other.version_;
                this->name_ = other.name_;
                this->description_ = other.description_;
                this->author_ = other.author_;
                this->module_path_ = other.module_path_;

                for (auto& alias : other.aliases_) {
                    this->aliases_[alias.first] = alias.second;
                }
            }

            ModuleInfo(ModuleInfo& other) {
                this->version_ = other.version_;
                this->name_ = other.name_;
                this->description_ = other.description_;
                this->author_ = other.author_;
                this->module_path_ = other.module_path_;

                for (auto& alias : other.aliases_) {
                    this->aliases_[alias.first] = alias.second;
                }
            }

            ModuleInfo(const std::filesystem::path& module_path, const std::string& name, const std::string& version,
                std::unordered_map<std::string, std::string> aliases,
                const std::string& author, const std::string& description)
                : name_(name),  version_(version),
                aliases_(aliases), author_(author),
                description_(description), module_path_(module_path) {}

            ModuleInfo(std::filesystem::path&& module_path, std::string&& name, std::string&& version,
                std::unordered_map<std::string, std::string>&& aliases,
                std::string&& author = "", std::string&& description = "")
                : name_(name),  version_(version),
                aliases_(aliases), author_(author),
                description_(description), module_path_(module_path) {}

            [[nodiscard]] static std::optional<ModuleInfo> fromJson(const std::string& module_path, const std::string& json);

            std::string name() const;
            std::string version() const;
            std::unordered_map<std::string, std::string> aliases() const;
            std::string author() const;
            std::string description() const;
            std::filesystem::path module_path() const;
        private:
            std::string name_;
            std::string version_;
            std::unordered_map<std::string, std::string> aliases_;
            std::string author_;
            std::string description_;
            std::filesystem::path module_path_;
        };

        class ModuleLoader {
        public:

            struct ModuleCommands {
                   ModuleCommands(int(*startf)(void(*)(const std::string&)),
                                  int(*stopf)(),
                                  std::string(*configf)()) :
                                  start_function_(startf),
                                  stop_function_(stopf),
                                  config_function_(configf) {}

                   int start(void(*writef)(const std::string&)) { return this->start_function_(writef); }
                   int stop() { return this->stop_function_(); }
                   std::string config() { return this->config_function_(); }
            private:
                int(*start_function_)(void(*)(const std::string&));
                int(*stop_function_)();
                std::string(*config_function_)();
            };

            ModuleLoader(std::shared_ptr<ModuleInfo> minfo) : module_info_(minfo) {}
            ~ModuleLoader() {
                spdlog::log(spdlog::level::warn, "{}: Calling the Module loader destructor(possibly closing library handle)", __FUNCTION__);
                if (this->library_)
                    dlclose(this->library_);
            }

            [[nodiscard]] std::optional<ModuleCommands> load_module();
        private:
            std::vector<bool> try_functions(std::vector<std::string>&& functions);

            void* library_;
            std::shared_ptr<ModuleInfo> module_info_;
        };

        class ModuleManager {
        public:
            ModuleManager(std::shared_ptr<ModuleInfo> minfo, std::shared_ptr<ModuleLoader> mloader) : module_loader_(mloader), module_info_(minfo) {
                if (!std::filesystem::exists(module_info_->module_path()))
                    throw std::invalid_argument("Module path does not exist");
            }

            [[nodiscard]] std::shared_ptr<ModuleInfo> get_info() const noexcept { return this->module_info_; }
            [[nodiscard]] std::shared_ptr<ModuleLoader> get_loader() const noexcept { return this->module_loader_; }
        private:
            std::shared_ptr<ModuleLoader> module_loader_;
            std::shared_ptr<ModuleInfo> module_info_;

        };
    } // namespace modules_interaction
} // namespace lit

#endif // LIT_MODULESINTERACTION_MODULESINTERACTION_HXX
