#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "libzippp.h"

namespace fs = std::filesystem;

void add_directory_to_archive(libzippp::ZipArchive& archive, const fs::path& dir_path, const fs::path& base_path) {
    for (const auto& entry : fs::recursive_directory_iterator(dir_path)) {
        const auto& path = entry.path();
        auto relative_path = fs::relative(path, base_path);

        if (fs::is_directory(path)) {
            archive.addEntry(relative_path.string() + "/");
        } else if (fs::is_regular_file(path)) {
            archive.addFile(relative_path.string(), path.string());
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_directory> <output_file.cmod>" << std::endl;
        return 1;
    }

    fs::path source_dir = argv[1];
    fs::path output_file = argv[2];

    if (!fs::is_directory(source_dir)) {
        std::cerr << "Error: Source directory does not exist or is not a directory." << std::endl;
        return 1;
    }

    fs::path src_subdir = source_dir / "src";
    fs::path info_subdir = source_dir / "info";

    if (!fs::is_directory(src_subdir) || !fs::is_directory(info_subdir)) {
        std::cerr << "Error: Source directory must contain 'src' and 'info' subdirectories." << std::endl;
        return 1;
    }

    try {
        libzippp::ZipArchive zf(output_file.string());
        zf.open(libzippp::ZipArchive::New);

        add_directory_to_archive(zf, src_subdir, source_dir);
        add_directory_to_archive(zf, info_subdir, source_dir);

        zf.close();
        std::cout << "Successfully created CMOD package: " << output_file << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error creating CMOD package: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}