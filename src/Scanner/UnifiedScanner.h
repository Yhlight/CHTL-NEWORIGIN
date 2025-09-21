#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class FragmentType {
    CSS,
    JavaScript, // For later
    CHTL_JS,    // For later
};

struct CodeFragment {
    FragmentType type;
    std::string content;
};

struct ScanResult {
    std::string chtlSource;
    std::map<std::string, CodeFragment> fragments; // placeholder -> fragment
};

class UnifiedScanner {
public:
    ScanResult Scan(const std::string& source);
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H
