#include "CHTLUnifiedScanner.h"

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(std::string source) : m_source(std::move(source)) {}

// This is a simplified implementation. A real-world scanner would need to be
// much more robust, handling nested comments, strings, etc.
size_t CHTLUnifiedScanner::findBlockEnd(size_t start_pos) {
    int brace_level = 1;
    for (size_t i = start_pos; i < m_source.length(); ++i) {
        if (m_source[i] == '{') {
            brace_level++;
        } else if (m_source[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

void CHTLUnifiedScanner::findAndReplaceScriptBlocks() {
    size_t pos = 0;
    while ((pos = m_source.find("script", pos)) != std::string::npos) {
        // Basic check to ensure it's a 'script {}' block
        size_t brace_pos = m_source.find('{', pos);
        // This check is very basic and could fail on e.g. `div { script-like-name ... }`
        // A more robust solution would use the actual CHTL lexer to find script blocks.
        // For now, this is a simple heuristic.
        std::string between = m_source.substr(pos + 6, brace_pos - (pos + 6));
        bool is_block = true;
        for(char c : between) {
            if (!isspace(c)) {
                is_block = false;
                break;
            }
        }

        if (is_block && brace_pos != std::string::npos) {
            size_t end_pos = findBlockEnd(brace_pos + 1);
            if (end_pos != std::string::npos) {
                // Found a block
                std::string placeholder_name = "__CHTL_SCRIPT_PLACEHOLDER_" + std::to_string(m_scriptCounter++) + "__";
                std::string replacement_text = "text { \"" + placeholder_name + "\" }";

                // Extract content (excluding the braces)
                std::string script_content = m_source.substr(brace_pos + 1, end_pos - brace_pos - 1);
                m_fragments.scriptBlocks[placeholder_name] = script_content;

                // Replace the entire block with the placeholder text block
                m_source.replace(pos, end_pos - pos + 1, replacement_text);

                // Start searching again from the position of the placeholder
                pos += replacement_text.length();
            } else {
                // Unmatched brace, move on
                pos++;
            }
        } else {
            // Not a script block, continue search
            pos++;
        }
    }
}


ScannedFragments CHTLUnifiedScanner::scan() {
    findAndReplaceScriptBlocks();
    m_fragments.primarySource = m_source;
    return m_fragments;
}

} // namespace CHTL
