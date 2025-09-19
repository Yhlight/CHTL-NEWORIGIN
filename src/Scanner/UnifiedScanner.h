#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>

namespace CHTL {

// A struct to hold the different code fragments after scanning.
struct CodeFragments {
    std::string chtl_part;
    std::string css_part;
    std::string js_part;
    std::string chtl_js_part;
};

class UnifiedScanner {
public:
    UnifiedScanner();
    CodeFragments scan(const std::string& filepath);

private:
    std::string readFile(const std::string& filepath);
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H
