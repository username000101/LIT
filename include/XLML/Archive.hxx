#ifndef LIT_XLML_ARCHIVE_HXX
#define LIT_XLML_ARCHIVE_HXX

#include <filesystem>
#include <string>

namespace lit {
    namespace xlml {
        std::filesystem::path extract_archive(const std::filesystem::path& path);
    }
}

#endif // LIT_XLML_ARCHIVE_HXX
