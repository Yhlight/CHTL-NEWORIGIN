#include "CHTLJSLexer.h"
#include <map>
#include <cctype>
#include <stdexcept>
#include <cstring> // For strchr

namespace CHTL_JS {

static const std::map<std::string, CHTLJSTokenType> KEYWORDS = {
    {"Listen", CHTLJSTokenType::KEYWORD_LISTEN},
};

CHTLJSLexer::CHTLJSLexer(std::string source) : m_source(std::move(source)) {}

char CHTLJSLexer::currentChar() const {
    if (m_position >= m_source.length()) return '\0';
    return m_source[m_position];
}

char CHTLJSLexer::peekChar() const {
    if (m_position + 1 >= m_source.length()) return '\0';
    return m_source[m_position + 1];
}

void CHTLJSLexer::advance(int count) {
    m_position += count;
}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type, const std::string& value) {
    return {type, value};
}

CHTLJSToken CHTLJSLexer::getNextToken() {
    // Skip whitespace
    while (isspace(currentChar())) {
        advance();
    }

    if (currentChar() == '\0') {
        return makeToken(CHTLJSTokenType::END_OF_FILE, "");
    }

    // Check for multi-character tokens
    if (currentChar() == '{' && peekChar() == '{') { advance(2); return makeToken(CHTLJSTokenType::DOUBLE_LEFT_BRACE, "{{"); }
    if (currentChar() == '}' && peekChar() == '}') { advance(2); return makeToken(CHTLJSTokenType::DOUBLE_RIGHT_BRACE, "}}"); }
    if (currentChar() == '-' && peekChar() == '>') { advance(2); return makeToken(CHTLJSTokenType::ARROW, "->"); }

    // Check for single-character tokens
    const char* single_chars = "{}:,";
    if (strchr(single_chars, currentChar())) {
        CHTLJSTokenType type;
        switch(currentChar()) {
            case '{': type = CHTLJSTokenType::LEFT_BRACE; break;
            case '}': type = CHTLJSTokenType::RIGHT_BRACE; break;
            case ':': type = CHTLJSTokenType::COLON; break;
            case ',': type = CHTLJSTokenType::COMMA; break;
            default: type = CHTLJSTokenType::UNKNOWN; break; // Should not happen
        }
        std::string val(1, currentChar());
        advance();
        return makeToken(type, val);
    }

    // If not a special symbol, it's an identifier, keyword, or function body
    size_t start = m_position;
    // This is a simplified function body parser. It assumes balanced parentheses for arrow functions.
    // A full implementation would be much more complex.
    bool is_function = false;
    if (currentChar() == '(') { // Potential arrow function
        int paren_level = 1;
        size_t current = m_position + 1;
        while(current < m_source.length() && paren_level > 0) {
            if(m_source[current] == '(') paren_level++;
            if(m_source[current] == ')') paren_level--;
            current++;
        }
        while(current < m_source.length() && isspace(m_source[current])) current++;
        if(current + 1 < m_source.length() && m_source[current] == '=' && m_source[current+1] == '>') {
            is_function = true;
        }
    }
    if (is_function || currentChar() == 'f') { // Potential function keyword
         // Heuristic: if it starts with 'function' or is an arrow function, parse as a block.
         if(m_source.substr(m_position, 8) == "function" || is_function) {
            int brace_level = 0;
            size_t current = m_position;
            // Find opening brace if it exists
            while(current < m_source.length() && m_source[current] != '{') current++;
            if(current < m_source.length()) { // Found a brace
                brace_level = 1;
                current++;
                while(current < m_source.length() && brace_level > 0) {
                    if (m_source[current] == '{') brace_level++;
                    if (m_source[current] == '}') brace_level--;
                    current++;
                }
                m_position = current;
                return makeToken(CHTLJSTokenType::FUNCTION_BODY, m_source.substr(start, current - start));
            }
         }
    }

    // Otherwise, it's an identifier or raw text
    while(currentChar() != '\0' && !isspace(currentChar()) && strchr("{}:,", currentChar()) == nullptr) {
        if ((currentChar() == '-' && peekChar() == '>')) break;
        advance();
    }

    std::string value = m_source.substr(start, m_position - start);
    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return makeToken(it->second, value);
    }

    return makeToken(CHTLJSTokenType::IDENTIFIER, value);
}

std::vector<CHTLJSToken> CHTLJSLexer::getAllTokens() {
    std::vector<CHTLJSToken> tokens;
    while (m_position < m_source.length()) {
        tokens.push_back(getNextToken());
    }
    tokens.push_back({CHTLJSTokenType::END_OF_FILE, ""});
    return tokens;
}

} // namespace CHTL_JS
