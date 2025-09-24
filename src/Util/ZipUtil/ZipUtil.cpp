#include "ZipUtil.h"
#include "miniz.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream> // FIX: Add missing include for std::ifstream

namespace CHTL::Util {

// Helper function to recursively add files to a zip archive
void add_to_zip_recursive(mz_zip_archive& zip_archive, const std::filesystem::path& base_dir, const std::filesystem::path& current_path) {
    for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
        auto relative_path = std::filesystem::relative(entry.path(), base_dir);
        if (entry.is_directory()) {
            // Add a trailing slash to directory names to ensure they are treated as directories.
            std::string dir_path_with_slash = relative_path.string() + "/";
            mz_zip_writer_add_mem(&zip_archive, dir_path_with_slash.c_str(), nullptr, 0, MZ_DEFAULT_COMPRESSION);
            add_to_zip_recursive(zip_archive, base_dir, entry.path());
        } else if (entry.is_regular_file()) {
            // Read file to buffer to avoid issues with file handles
            std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(size);
            if (file.read(buffer.data(), size))
            {
                mz_zip_writer_add_mem(&zip_archive, relative_path.string().c_str(), buffer.data(), buffer.size(), MZ_DEFAULT_COMPRESSION);
            }
        }
    }
}

bool ZipUtil::zip(const std::filesystem::path& sourceDir, const std::filesystem::path& zipPath) {
    if (!std::filesystem::exists(sourceDir) || !std::filesystem::is_directory(sourceDir)) {
        return false;
    }

    mz_zip_archive zip_archive = {};
    if (!mz_zip_writer_init_file(&zip_archive, zipPath.string().c_str(), 0)) {
        return false;
    }

    add_to_zip_recursive(zip_archive, sourceDir, sourceDir);

    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);
    return true;
}

bool ZipUtil::unzip(const std::filesystem::path& zipPath, const std::filesystem::path& destinationDir) {
    if (!std::filesystem::exists(zipPath) || !std::filesystem::is_regular_file(zipPath)) {
        return false;
    }

    std::filesystem::create_directories(destinationDir);

    mz_zip_archive zip_archive = {};
    if (!mz_zip_reader_init_file(&zip_archive, zipPath.string().c_str(), 0)) {
        return false;
    }

    for (mz_uint i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        std::filesystem::path dest_path = destinationDir / file_stat.m_filename;

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            std::filesystem::create_directories(dest_path);
        } else {
            if (dest_path.has_parent_path()) {
                std::filesystem::create_directories(dest_path.parent_path());
            }
            if (!mz_zip_reader_extract_to_file(&zip_archive, i, dest_path.string().c_str(), 0)) {
                mz_zip_reader_end(&zip_archive);
                return false;
            }
        }
    }

    mz_zip_reader_end(&zip_archive);
    return true;
}

} // namespace CHTL::Util
