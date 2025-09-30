#include "packer.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// The main function now simply parses arguments and calls the core packer function.
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_directory> <output_file.cmod>" << std::endl;
        return 1;
    }

    fs::path source_dir = argv[1];
    fs::path output_file = argv[2];

    return pack_cmod(source_dir, output_file);
}