#include "UnifiedScanner.h"

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    std::string::size_type lastPos = 0;
    std::string::size_type scriptStart = source.find("script {");

    while (scriptStart != std::string::npos) {
        // Add the CHTL fragment before the script block
        if (scriptStart > lastPos) {
            fragments.push_back({FragmentType::CHTL, source.substr(lastPos, scriptStart - lastPos)});
        }

        // Find the end of the script block
        std::string::size_type bracePos = scriptStart + 8;
        int braceCount = 1;
        while (braceCount > 0 && bracePos < source.length()) {
            if (source[bracePos] == '{') {
                braceCount++;
            } else if (source[bracePos] == '}') {
                braceCount--;
            }
            bracePos++;
        }

        // Add the JS fragment
        fragments.push_back({FragmentType::JS, source.substr(scriptStart + 8, bracePos - scriptStart - 9)});

        lastPos = bracePos;
        scriptStart = source.find("script {", lastPos);
    }

    // Add the remaining CHTL fragment
    if (lastPos < source.length()) {
        fragments.push_back({FragmentType::CHTL, source.substr(lastPos)});
    }

    return fragments;
}