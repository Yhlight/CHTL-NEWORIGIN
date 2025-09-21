#include "UnifiedScanner.h"
#include <regex>

namespace CHTL {

ScanResult UnifiedScanner::Scan(const std::string& source) {
    ScanResult result;
    std::string processedSource = source;
    size_t nextPlaceholderId = 0;

    // This is a naive implementation for now to get the architecture in place.
    // A proper implementation needs to handle nested blocks, comments, and strings correctly.
    size_t startPos = 0;
    while ((startPos = processedSource.find("style {", startPos)) != std::string::npos) {

        size_t contentStartPos = startPos + 7; // After "style {"
        int braceCount = 1;
        size_t searchPos = contentStartPos;
        size_t contentEndPos = std::string::npos;

        while (braceCount > 0 && searchPos < processedSource.length()) {
            if (processedSource[searchPos] == '{') {
                braceCount++;
            } else if (processedSource[searchPos] == '}') {
                braceCount--;
            }
            if (braceCount == 0) {
                contentEndPos = searchPos;
            }
            searchPos++;
        }

        if (contentEndPos == std::string::npos) {
            // Malformed style block, stop scanning.
            break;
        }

        // Extract the content of the style block
        std::string styleContent = processedSource.substr(contentStartPos, contentEndPos - contentStartPos);

        // Create a placeholder that is also a valid CHTL text node
        std::string placeholder_key = "__CHTL_CSS_PLACEHOLDER_" + std::to_string(nextPlaceholderId) + "__";
        std::string placeholder_node = "text { \"" + placeholder_key + "\" }";
        nextPlaceholderId++;

        // Store the fragment
        result.fragments[placeholder_key] = {FragmentType::CSS, styleContent};

        // Replace the style block with the placeholder
        size_t blockEndPos = contentEndPos + 1;
        processedSource.replace(startPos, blockEndPos - startPos, placeholder_node);

        // Continue searching from the position of the placeholder
        startPos = startPos + placeholder_node.length();
    }

    result.chtlSource = processedSource;
    return result;
}

} // namespace CHTL
