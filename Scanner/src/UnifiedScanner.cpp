#include "UnifiedScanner.hpp"

namespace chtl::scanner {

std::vector<Fragment> UnifiedScanner::scan(const std::string& source) const {
    // Extract script blocks (all) and leave others as CHTL for now.
    std::vector<Fragment> out;
    size_t i = 0;
    auto pushChtl = [&](size_t from, size_t to) {
        if (to > from) out.push_back({ FragmentType::Chtl, source.substr(from, to - from) });
    };
    while (i < source.size()) {
        size_t found = source.find("script", i);
        if (found == std::string::npos) {
            pushChtl(i, source.size());
            break;
        }
        // word boundary check
        bool leftOk = (found == 0) || !std::isalnum(static_cast<unsigned char>(source[found - 1]));
        size_t endWord = found + 6;
        bool rightOk = (endWord >= source.size()) || !std::isalnum(static_cast<unsigned char>(source[endWord]));
        if (!(leftOk && rightOk)) { i = found + 1; continue; }
        // skip whitespace to '{'
        size_t bracePos = endWord;
        while (bracePos < source.size() && std::isspace(static_cast<unsigned char>(source[bracePos]))) ++bracePos;
        if (bracePos >= source.size() || source[bracePos] != '{') { i = endWord; continue; }
        // match braces accounting for nested braces, strings, and comments
        size_t startBlock = found;
        size_t j = bracePos;
        int depth = 0;
        bool inS = false, inD = false, inLC = false, inBC = false;
        while (j < source.size()) {
            char c = source[j];
            char prev = j ? source[j - 1] : '\0';
            if (!inS && !inD && !inLC && !inBC) {
                if (c == '"') { inD = true; }
                else if (c == '\'') { inS = true; }
                else if (c == '/' && j + 1 < source.size() && source[j + 1] == '/') { inLC = true; ++j; }
                else if (c == '/' && j + 1 < source.size() && source[j + 1] == '*') { inBC = true; ++j; }
                else if (c == '{') { ++depth; }
                else if (c == '}') { --depth; if (depth == 0) { ++j; break; } }
            } else if (inD) {
                if (c == '"' && prev != '\\') inD = false;
            } else if (inS) {
                if (c == '\'' && prev != '\\') inS = false;
            } else if (inLC) {
                if (c == '\n') inLC = false;
            } else if (inBC) {
                if (c == '*' && j + 1 < source.size() && source[j + 1] == '/') { inBC = false; ++j; }
            }
            ++j;
        }
        // Emit preceding CHTL and the script placeholder block
        pushChtl(i, startBlock);
        std::string block = source.substr(startBlock, j - startBlock);
        out.push_back({ FragmentType::JsPlaceholder, block });
        i = j;
    }
    return out;
}

}
