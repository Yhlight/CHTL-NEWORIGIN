#include "CHTLJSLexer.h"

namespace CHTL_JS {

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

CHTLJSToken CHTLJSLexer::getNextToken() {
    if (currentChar() == '\0') {
        return {CHTLJSTokenType::END_OF_FILE, ""};
    }

    // Check for CHTL JS special syntax
    if (currentChar() == '{' && peekChar() == '{') {
        advance(2);
        return {CHTLJSTokenType::DOUBLE_LEFT_BRACE, "{{"};
    }
    if (currentChar() == '}' && peekChar() == '}') {
        advance(2);
        return {CHTLJSTokenType::DOUBLE_RIGHT_BRACE, "}}"};
    }
    if (currentChar() == '-' && peekChar() == '>') {
        advance(2);
        return {CHTLJSTokenType::ARROW, "->"};
    }

    // If not special syntax, read until the next special syntax or EOF
    std::string raw_js;
    size_t start = m_position;
    while(currentChar() != '\0') {
        if ((currentChar() == '{' && peekChar() == '{') ||
            (currentChar() == '}' && peekChar() == '}') ||
            (currentChar() == '-' && peekChar() == '>')) {
            break;
        }
        advance();
    }

    raw_js = m_source.substr(start, m_position - start);
    // This simple lexer will tokenize selector content as RAW_JS.
    // The parser will need to handle this.
    return {CHTLJSTokenType::RAW_JS, raw_js};
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
