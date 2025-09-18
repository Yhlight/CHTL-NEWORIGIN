#include "CHTLJSParser.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJSNode.h"
#include "../CHTLJSNode/ListenNode.h"
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

std::unique_ptr<CHTLJS_BaseNode> CHTLJSParser::parseEnhancedSelector() {
    advance(); // Consume '{{'
    if (currentToken().type != CHTLJSTokenType::IDENTIFIER) {
         throw std::runtime_error("Expected selector content inside {{...}}");
    }
    std::string selector = currentToken().value;
    advance(); // Consume content
    if (currentToken().type != CHTLJSTokenType::DOUBLE_RIGHT_BRACE) {
        throw std::runtime_error("Expected '}}' to close selector.");
    }
    advance(); // Consume '}}'
    return std::make_unique<EnhancedSelectorNode>(selector);
}

std::unique_ptr<CHTLJS_BaseNode> CHTLJSParser::parseListenBlock(std::unique_ptr<CHTLJS_BaseNode> target) {
    advance(); // Consume 'Listen'
    if (currentToken().type != CHTLJSTokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' after 'Listen' keyword.");
    }
    advance(); // Consume '{'

    std::map<std::string, std::string> events;
    while(currentToken().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (currentToken().type != CHTLJSTokenType::IDENTIFIER) {
            throw std::runtime_error("Expected event name identifier (e.g., 'click').");
        }
        std::string eventName = currentToken().value;
        advance();

        if (currentToken().type != CHTLJSTokenType::COLON) {
            throw std::runtime_error("Expected ':' after event name.");
        }
        advance();

        if (currentToken().type != CHTLJSTokenType::FUNCTION_BODY) {
            throw std::runtime_error("Expected function body for event handler.");
        }
        std::string body = currentToken().value;
        advance();
        events[eventName] = body;

        if (currentToken().type == CHTLJSTokenType::COMMA) {
            advance();
        }
    }
    advance(); // Consume '}'
    return std::make_unique<ListenNode>(std::move(target), std::move(events));
}

std::unique_ptr<CHTLJS_BaseNode> CHTLJSParser::parseStatement() {
    std::unique_ptr<CHTLJS_BaseNode> left;

    if (currentToken().type == CHTLJSTokenType::DOUBLE_LEFT_BRACE) {
        left = parseEnhancedSelector();
    } else {
        // If it's not a special construct, treat it as a raw JS node to be passed through.
        // This is a simplification; a full parser would handle JS expressions.
        left = std::make_unique<RawJSNode>(currentToken().value);
        advance();
        return left;
    }

    // Check for infix operators like ->
    if (currentToken().type == CHTLJSTokenType::ARROW) {
        advance(); // Consume '->'
        if (currentToken().type == CHTLJSTokenType::KEYWORD_LISTEN) {
            return parseListenBlock(std::move(left));
        } else {
            // Re-combine the arrow with the next identifier to form a raw JS block
            // This is a workaround for not having a full JS expression parser.
            std::string raw_text = "->" + currentToken().value;
            advance();
            return std::make_unique<RawJSNode>(raw_text);
        }
    }

    return left;
}

std::vector<std::unique_ptr<CHTLJS_BaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJS_BaseNode>> nodes;
    while(currentToken().type != CHTLJSTokenType::END_OF_FILE) {
        nodes.push_back(parseStatement());
    }
    return nodes;
}

} // namespace CHTL_JS
