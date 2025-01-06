#ifndef LIT_MODULESINTERACTION_MODULESINTERACTION_HXX
#define LIT_MODULESINTERACTION_MODULESINTERACTION_HXX

#include <dlfcn.h>

#include <csignal>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>

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
    }
}

#endif // LIT_MODULESINTERACTION_MODULESINTERACTION_HXX
