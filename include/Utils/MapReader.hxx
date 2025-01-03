#ifndef LIT_UTILS_MAPREADER_HXX
#define LIT_UTILS_MAPREADER_HXX

#include <sstream>

namespace utils {
    template <typename MapType> class MapReader {
    public:
        MapReader(MapType&& m) : m_(m) {}
        MapReader(MapType& m) : m_(m) {}

        std::string read_map() {
            if (this->m_.empty())
                return "";
            std::ostringstream oss;
            std::for_each(this->m_.begin(), this->m_.end(), [&oss](auto& p) { oss << "[" << p.first << ":" << p.second << "] "; });
            return oss.str();
        }
    private:
        MapType m_;
    };
}

#endif // LIT_UTILS_MAPREADER_HXX
