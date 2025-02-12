#include <Utils/TerminalIO.hxx>

#include <string>
#include <unordered_set>

std::string utils::terminal_io::console_choice(const std::string& promt, std::unordered_set<std::string> choice_variants) {
    while (true) {
        std::cout << promt << std::endl;
        std::string buffer;
        std::getline(std::cin, buffer);
        if (choice_variants.empty())
            return buffer;
        else if (choice_variants.count(buffer) > 0)
            return buffer;
        else
            continue;
    }
}
