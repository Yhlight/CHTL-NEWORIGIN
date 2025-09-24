#include "UnifiedScanner.h"
#include <sstream>

namespace CHTL {

ScanResult UnifiedScanner::Scan(const std::string& source) {
    std::stringstream chtlCodeBuilder;
    std::stringstream globalCssBuilder;

    int braceLevel = 0;
    size_t i = 0;
    size_t last_copy_pos = 0;

    while (i < source.length()) {
        // Simple check for top-level 'style' keyword
        if (braceLevel == 0 && source.substr(i, 5) == "style") {
            size_t style_keyword_end = i + 5;
            size_t block_start = source.find('{', style_keyword_end);

            if (block_start != std::string::npos) {
                // Check if there's only whitespace between "style" and "{"
                bool onlyWhitespace = true;
                for (size_t j = style_keyword_end; j < block_start; ++j) {
                    if (!isspace(source[j])) {
                        onlyWhitespace = false;
                        break;
                    }
                }

                if (onlyWhitespace) {
                    // We found a global style block.
                    // Copy the CHTL code before this block.
                    chtlCodeBuilder << source.substr(last_copy_pos, i - last_copy_pos);

                    int innerBraceLevel = 1;
                    size_t content_start = block_start + 1;
                    size_t content_end = content_start;
                    while (content_end < source.length() && innerBraceLevel > 0) {
                        if (source[content_end] == '{') {
                            innerBraceLevel++;
                        } else if (source[content_end] == '}') {
                            innerBraceLevel--;
                        }
                        content_end++;
                    }

                    if (innerBraceLevel == 0) {
                        // Extract content and move pointers
                        globalCssBuilder << source.substr(content_start, content_end - content_start - 1);
                        i = content_end;
                        last_copy_pos = i;
                        continue;
                    }
                }
            }
        }

        // Track brace level for the main CHTL code
        if (source[i] == '{') {
            braceLevel++;
        } else if (source[i] == '}') {
            if (braceLevel > 0) { // handle potential malformed code
                braceLevel--;
            }
        }
        i++;
    }

    // Append any remaining CHTL code
    if (last_copy_pos < source.length()) {
        chtlCodeBuilder << source.substr(last_copy_pos);
    }

    return {chtlCodeBuilder.str(), globalCssBuilder.str()};
}

} // namespace CHTL
