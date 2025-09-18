#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// Holds the result of a scan, separating the main source from other language blocks.
struct ScannedFragments {
    std::string primarySource; // The main source with placeholders
    std::map<std::string, std::string> scriptBlocks; // Map of placeholder -> script content
};

// Scans a source file and separates different language blocks.
class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(std::string source);

    ScannedFragments scan();

private:
    void findAndReplaceScriptBlocks();
    size_t findBlockEnd(size_t start_pos);

    std::string m_source;
    ScannedFragments m_fragments;
    int m_scriptCounter = 0;
};

} // namespace CHTL
