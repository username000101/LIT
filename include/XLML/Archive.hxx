#ifndef LIT_XLML_ARCHIVE_HXX
#define LIT_XLML_ARCHIVE_HXX

#include <filesystem>
#include <string>

#include <Utils/Macros.hxx>

namespace lit {
    namespace xlml {
        std::filesystem::path LIT_EXPORT extract_archive(const std::filesystem::path& path);
    }
}

#endif // LIT_XLML_ARCHIVE_HXX
