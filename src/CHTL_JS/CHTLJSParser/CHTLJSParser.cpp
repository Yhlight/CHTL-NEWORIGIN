#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include <stdexcept>

CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken> tokens) : tokens(std::move(tokens)) {}

CHTLJSToken CHTLJSParser::currentToken() const {
    if (position >= tokens.size()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }
    return tokens[position];
}

void CHTLJSParser::advance() {
    if (position < tokens.size()) {
        position++;
    }
}

std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSNode>> nodes;

    while (currentToken().type != CHTLJSTokenType::EndOfFile) {
        switch (currentToken().type) {
            case CHTLJSTokenType::OpenDoubleBrace: {
                advance(); // Consume '{{'
                if (currentToken().type != CHTLJSTokenType::RawJS) {
                    throw std::runtime_error("Expected raw text inside enhanced selector.");
                }
                std::string selector_text = currentToken().value;
                nodes.push_back(std::make_unique<CHTLJSEnhancedSelectorNode>(selector_text));
                advance(); // Consume selector text
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) {
                    throw std::runtime_error("Unclosed enhanced selector.");
                }
                advance(); // Consume '}}'
                break;
            }
            case CHTLJSTokenType::RawJS:
                nodes.push_back(std::make_unique<RawJavaScriptNode>(currentToken().value));
                advance();
                break;
            default:
                // For now, we'll treat other tokens as raw JS to be safe.
                nodes.push_back(std::make_unique<RawJavaScriptNode>(currentToken().value));
                advance();
                break;
        }
    }

    return nodes;
}