#ifndef LIT_CONFIGURATION_LITCONFIGURATION_HXX
#define LIT_CONFIGURATION_LITCONFIGURATION_HXX

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#include <td/telegram/td_api.h>

#include <ModulesInteraction/ModulesInteraction.hxx>

namespace lit {
    namespace cfg {
        struct LITCfg {
            LITCfg(const std::string& dir, const std::string& version, const std::string& api_id, const std::string& api_hash,
                   std::vector<td::td_api::int53> blocked_requests)
                 : dir_(dir), version_(version), api_id_(api_id), api_hash_(api_hash),
                   blocked_requests_(blocked_requests) {}

            std::string dir() { return this->dir_; }
            std::string version() { return this->version_; }
            std::string api_id() { return this->api_id_; }
            std::string api_hash() { return this->api_hash_; }
            std::vector<td::td_api::int53> blocked_requests() { return this->blocked_requests_; }

        private:
            std::string dir_;
            std::string version_;
            std::string api_id_;
            std::string api_hash_;
            std::vector<td::td_api::int53> blocked_requests_;
        };

        LITCfg load_config(const std::filesystem::path& file = LIT_CONFIG);
    }
}

#endif // LIT_CONFIGURATIONLITCONFIGURATION_HXX
