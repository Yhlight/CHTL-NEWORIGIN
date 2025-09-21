#include "Loader.h"
#include "CMODSystem/CMODSystem.h"
#include "miniz.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

std::string Loader::loadSource(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: File not found: " << path << std::endl;
        return "";
    }
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Loader::loadFromCMOD(const std::filesystem::path& cmodPath, const std::string& internalPath) {
    mz_zip_archive zip_archive = {};
    if (!mz_zip_reader_init_file(&zip_archive, cmodPath.string().c_str(), 0)) {
        std::cerr << "Error: Failed to open CMOD file: " << cmodPath << std::endl;
        return "";
    }

    size_t uncompressed_size;
    void* p = mz_zip_reader_extract_file_to_heap(&zip_archive, internalPath.c_str(), &uncompressed_size, 0);
    if (!p) {
        std::cerr << "Error: Failed to extract file '" << internalPath << "' from CMOD '" << cmodPath << "'" << std::endl;
        mz_zip_reader_end(&zip_archive);
        return "";
    }

    std::string content(static_cast<char*>(p), uncompressed_size);

    mz_free(p);
    mz_zip_reader_end(&zip_archive);

    return content;
}

} // namespace CHTL
