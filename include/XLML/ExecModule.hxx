#ifndef LIT_XLML_EXECMODULE_HXX
#define LIT_XLML_EXECMODULE_HXX

#include <memory>
#include <string>
#include <vector>

#include <td/telegram/td_api.h>


namespace lit {
    namespace xlml {
        bool start_module(std::string message_text, std::vector<std::string> args,
                          std::shared_ptr<td::td_api::message>);
    }
}

#endif // LIT_XLML_EXECMODULE_HXX
