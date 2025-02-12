#include <XLML/Archive.hxx>

#include <fstream>

#include <archive.h>
#include <archive_entry.h>
#include <spdlog/spdlog.h>

#include <Utils/Macros.hxx>

std::filesystem::path lit::xlml::extract_archive(const std::filesystem::path& path) {
    static auto logger = spdlog::get("XLML");
    ASSERT(logger, "The 'XLML' logger is not initialized");

    if (path.empty()) {
        logger->log(spdlog::level::err,
            "{}: Failed to extract archive: the path is empty",
            __PRETTY_FUNCTION__);
        return "";
    }

    auto target_dir = "/tmp/lit/xlml/" + path.filename().stem().string();
    std::filesystem::create_directories(target_dir);


    archive* archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_filter_all(archive);

    if (archive_read_open_filename(archive, path.string().c_str(), 10240) != ARCHIVE_OK) {
        logger->log(spdlog::level::err,
            "{}: Failed to extract archive: libarchive returned error: {}",
            __PRETTY_FUNCTION__, archive_error_string(archive));
        archive_read_free(archive);
        return "";
    }

    archive_entry* entry;
    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
        auto archive_file_path = archive_entry_pathname(entry);
        auto target_file = target_dir + "/" + archive_file_path;
        logger->log(spdlog::level::trace,
            "{}: Extracting '{}' to '{}' from '{}'",
            __PRETTY_FUNCTION__, archive_file_path, target_file, path.string());

        std::ofstream write(target_file, std::ios::binary);
        if (!write.is_open()) {
            logger->log(spdlog::level::warn,
                "{}: Failed to extract '{}': failed to open stream",
                __PRETTY_FUNCTION__, archive_file_path);
            archive_read_data_skip(archive);
            continue;
        }

        const void* buffer;
        std::size_t size;
        la_int64_t offset;
        while (archive_read_data_block(archive, &buffer, &size, &offset) == ARCHIVE_OK) {
            write.write(static_cast<const char*>(buffer), size);
        }
        write.close();
    }
    archive_read_free(archive);
    return target_dir;
}
