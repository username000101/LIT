#include <Auth/TDLibAuthentification.hxx>

#include <td/telegram/td_api.h>

#include <Utils/StringsBinder.hxx>
#include <Utils/TerminalIO.hxx>

bool lit::td_auth::auth_in_account(std::shared_ptr<td::ClientManager> client, td::ClientManager::ClientId client_id) {
    using utils::terminal_io::bgColor;
    using utils::terminal_io::fgColor;
    std::system("clear");
    while (true) {
        std::string phone_number = utils::terminal_io::console_input
        (utils::bindStrings(fgColor["blue"], "Write your phone:>"));
        if (phone_number.empty()) {
            std::cout << fgColor["red"] << "Incorrect input, please try again" << fgColor["reset"] << std::endl;
            continue;
        }
    }

}
