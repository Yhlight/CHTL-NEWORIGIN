#pragma once

#include "Scanner/ScanResult.h"
#include <string>

namespace CHTL {

class UnifiedScanner {
public:
    ScanResult scan(const std::string& filePath);
};

} // namespace CHTL
