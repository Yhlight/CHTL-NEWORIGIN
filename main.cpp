#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "CHTL/Compiler.h"
#include "CHTL/CMODSystem/CMODSystem.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--pack") {
        if (argc != 4) {
            std::cerr << "Usage: chtl_compiler --pack <input_dir> <output_file.cmod>" << std::endl;
            return 1;
        }
        std::string inputDir = argv[2];
        std::string outputFile = argv[3];
        if (CHTL::CMODSystem::pack(inputDir, outputFile)) {
            return 0;
        } else {
            return 1;
        }
    }

    if (argc != 2) {
        std::cerr << "Usage: chtl_compiler <file.chtl>" << std::endl;
        return 1;
    }

    CHTL::Compiler compiler;
    compiler.compile(argv[1]);

    return 0;
}
