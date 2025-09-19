#include "Scanner/UnifiedScanner.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace CHTL {

UnifiedScanner::UnifiedScanner() {
    // Constructor
}

std::string UnifiedScanner::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

CodeFragments UnifiedScanner::scan(const std::string& filepath) {
    std::string content = readFile(filepath);
    // For now, assume the whole file is CHTL.
    // The actual splitting logic will be implemented later.
    return {content, "", "", ""};
}

} // namespace CHTL
