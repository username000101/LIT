#ifndef LIT_UTILS_VECTORREADER_HXX
#define LIT_UTILS_VECTORREADER_HXX

#include <iostream>
#include <sstream>
#include <vector>

namespace utils {
    template <typename VectorValueType> class VectorReader {
    public:
        VectorReader(std::vector<VectorValueType> v) : v_(v) {}

        std::string read_vector() {
            if (this->v_.empty())
                return "";
            std::ostringstream oss;
            std::for_each(this->v_.begin(), this->v_.end(), [&oss](auto& v) { oss << v << " "; });
            return oss.str();
        }
    private:
        std::vector<VectorValueType> v_;
    };
}

#endif // LIT_UTILS_VECTORREADER_HXX
