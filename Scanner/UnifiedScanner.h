#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>

namespace CHTL {

struct ScanResult {
    std::string chtlCode;
    std::string globalCss;
    // std::string globalJs; // For the future
};

class UnifiedScanner {
public:
    ScanResult Scan(const std::string& source);
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H
