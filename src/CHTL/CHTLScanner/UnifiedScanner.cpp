#include "UnifiedScanner.h"
#include <stdexcept>

UnifiedScanner::UnifiedScanner(const std::string& source)
    : source(source), position(0) {}

// Finds the end of a matched block (e.g., {...}), handling nested blocks.
size_t findBlockEnd(const std::string& source, size_t startPos) {
    int braceLevel = 1; // Start at 1 since the opening brace is already found
    bool inString = false;
    char stringChar = '\0';

    for (size_t i = startPos; i < source.length(); ++i) {
        char c = source[i];

        if (inString) {
            if (c == '\\' && i + 1 < source.length()) {
                i++; // Skip escaped character
            } else if (c == stringChar) {
                inString = false;
            }
            continue;
        }

        if (c == '"' || c == '\'') {
            inString = true;
            stringChar = c;
        } else if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
            if (braceLevel == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

std::vector<std::unique_ptr<CodeFragment>> UnifiedScanner::scan() {
    std::vector<std::unique_ptr<CodeFragment>> fragments;
    std::string chtlContent;
    size_t lastFragmentEnd = 0;

    while (position < source.length()) {
        size_t stylePos = source.find("style", position);
        size_t scriptPos = source.find("script", position);

        size_t nextBlockPos = std::string::npos;
        CodeFragment::Language nextBlockLang = CodeFragment::Language::UNKNOWN;
        std::string keyword;

        if (stylePos != std::string::npos && (scriptPos == std::string::npos || stylePos < scriptPos)) {
            nextBlockPos = stylePos;
            nextBlockLang = CodeFragment::Language::CSS;
            keyword = "style";
        } else if (scriptPos != std::string::npos) {
            nextBlockPos = scriptPos;
            nextBlockLang = CodeFragment::Language::JS;
            keyword = "script";
        }

        if (nextBlockPos == std::string::npos) {
            break; // No more blocks
        }

        size_t openBracePos = source.find('{', nextBlockPos + keyword.length());
        if (openBracePos == std::string::npos) {
            position = nextBlockPos + 1;
            continue;
        }

        std::string between = source.substr(nextBlockPos + keyword.length(), openBracePos - (nextBlockPos + keyword.length()));
        if (between.find_first_not_of(" \t\n\r") != std::string::npos) {
            position = nextBlockPos + 1;
            continue;
        }

        size_t blockEndPos = findBlockEnd(source, openBracePos + 1);
        if (blockEndPos == std::string::npos) {
            throw std::runtime_error("Unmatched braces in source file.");
        }

        // Add the CHTL content before the block
        chtlContent += source.substr(lastFragmentEnd, nextBlockPos - lastFragmentEnd);

        // Add a placeholder for the block to the CHTL content
        if (nextBlockLang == CodeFragment::Language::CSS) {
            chtlContent += "/* CHTL_CSS_PLACEHOLDER */";
        } else {
            chtlContent += "/* CHTL_JS_PLACEHOLDER */";
        }

        // Create the fragment for the block's content
        auto blockFragment = std::make_unique<CodeFragment>();
        blockFragment->lang = nextBlockLang;
        blockFragment->content = source.substr(openBracePos + 1, blockEndPos - (openBracePos + 1));
        fragments.push_back(std::move(blockFragment));

        position = blockEndPos + 1;
        lastFragmentEnd = position;
    }

    // Add the remaining CHTL content
    chtlContent += source.substr(lastFragmentEnd);

    // Create the main CHTL fragment
    auto chtlFragment = std::make_unique<CodeFragment>();
    chtlFragment->lang = CodeFragment::Language::CHTL;
    chtlFragment->content = chtlContent;
    // Insert the CHTL fragment at the beginning
    fragments.insert(fragments.begin(), std::move(chtlFragment));

    return fragments;
}