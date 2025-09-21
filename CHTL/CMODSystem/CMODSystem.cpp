#include "CMODSystem.h"
#include "miniz.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

namespace CHTL {

// Helper function to read a file into a vector of bytes
static std::vector<char> readFileContents(const fs::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        return {};
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return {};
    }
    return buffer;
}


bool CMODSystem::pack(const std::string& modulePath, const std::string& outputPath) {
    if (!fs::is_directory(modulePath)) {
        std::cerr << "Error: Module path '" << modulePath << "' is not a valid directory." << std::endl;
        return false;
    }

    mz_zip_archive zip_archive = {};
    if (!mz_zip_writer_init_file(&zip_archive, outputPath.c_str(), 0)) {
        std::cerr << "Error: Failed to initialize zip archive writer." << std::endl;
        return false;
    }

    try {
        fs::path base_path(modulePath);
        for (const auto& entry : fs::recursive_directory_iterator(base_path)) {
            if (fs::is_regular_file(entry.path())) {
                fs::path relative_path = fs::relative(entry.path(), base_path);
                std::vector<char> file_contents = readFileContents(entry.path());
                if (file_contents.empty() && fs::file_size(entry.path()) > 0) {
                     std::cerr << "Warning: Could not read file or file is empty: " << entry.path() << std::endl;
                     continue;
                }

                if (!mz_zip_writer_add_mem(&zip_archive, relative_path.string().c_str(), file_contents.data(), file_contents.size(), MZ_DEFAULT_COMPRESSION)) {
                    std::cerr << "Error: Failed to add file to zip: " << relative_path << std::endl;
                    mz_zip_writer_end(&zip_archive);
                    return false;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        mz_zip_writer_end(&zip_archive);
        return false;
    }


    if (!mz_zip_writer_finalize_archive(&zip_archive)) {
        std::cerr << "Error: Failed to finalize zip archive." << std::endl;
        mz_zip_writer_end(&zip_archive);
        return false;
    }

    if (!mz_zip_writer_end(&zip_archive)) {
         std::cerr << "Error: Failed to close zip writer." << std::endl;
        return false;
    }

    std::cout << "Successfully created CMOD module: " << outputPath << std::endl;
    return true;
}

bool CMODSystem::unpack(const std::string& cmodPath, const std::string& extractPath) {
    mz_zip_archive zip_archive = {};
    if (!mz_zip_reader_init_file(&zip_archive, cmodPath.c_str(), 0)) {
        std::cerr << "Error: Failed to open CMOD file: " << cmodPath << std::endl;
        return false;
    }

    mz_uint num_files = mz_zip_reader_get_num_files(&zip_archive);
    if (num_files == 0) {
        mz_zip_reader_end(&zip_archive);
        return true; // Empty archive is not an error
    }

    fs::create_directories(extractPath);

    for (mz_uint i = 0; i < num_files; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            std::cerr << "Error: Failed to get file stats from CMOD." << std::endl;
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        fs::path out_path = fs::path(extractPath) / fs::path(file_stat.m_filename);

        if (out_path.has_parent_path()) {
            fs::create_directories(out_path.parent_path());
        }

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue; // Directory already created by parent_path logic or is empty
        }

        if (!mz_zip_reader_extract_to_file(&zip_archive, i, out_path.string().c_str(), 0)) {
            std::cerr << "Error: Failed to extract file: " << file_stat.m_filename << std::endl;
            mz_zip_reader_end(&zip_archive);
            return false;
        }
    }

    if (!mz_zip_reader_end(&zip_archive)) {
        std::cerr << "Error: Failed to close CMOD reader." << std::endl;
        return false;
    }

    return true;
}


} // namespace CHTL
