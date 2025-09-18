#include "CHTLJSParser.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJSNode.h"
#include <stdexcept>

namespace CHTL_JS {

CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken> tokens) : m_tokens(std::move(tokens)) {}

const CHTLJSToken& CHTLJSParser::currentToken() const {
    if (m_position >= m_tokens.size()) {
        static CHTLJSToken eof{CHTLJSTokenType::END_OF_FILE, ""};
        return eof;
    }
    return m_tokens[m_position];
}

void CHTLJSParser::advance() {
    if (m_position < m_tokens.size()) {
        m_position++;
    }
}

std::vector<std::unique_ptr<CHTLJS_BaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJS_BaseNode>> nodes;

    while(currentToken().type != CHTLJSTokenType::END_OF_FILE) {
        switch(currentToken().type) {
            case CHTLJSTokenType::DOUBLE_LEFT_BRACE: {
                advance(); // Consume '{{'
                if (currentToken().type != CHTLJSTokenType::RAW_JS) {
                    throw std::runtime_error("Expected selector content inside {{...}}");
                }
                std::string selector = currentToken().value;
                advance(); // Consume content
                if (currentToken().type != CHTLJSTokenType::DOUBLE_RIGHT_BRACE) {
                    throw std::runtime_error("Expected '}}' to close selector.");
                }
                advance(); // Consume '}}'
                nodes.push_back(std::make_unique<EnhancedSelectorNode>(selector));
                break;
            }
            case CHTLJSTokenType::RAW_JS: {
                nodes.push_back(std::make_unique<RawJSNode>(currentToken().value));
                advance();
                break;
            }
            // Other token types like ARROW would be handled here in the future
            default:
                // For now, just advance past unhandled tokens
                advance();
                break;
        }
    }

    return nodes;
}

} // namespace CHTL_JS
