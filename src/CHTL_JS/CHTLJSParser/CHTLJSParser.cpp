#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/ListenNode.h"
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
            case CHTLJSTokenType::Arrow: {
                advance(); // Consume '->'
                if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "Listen") {
                    nodes.push_back(parseListenBlock());
                } else {
                    // Handle other arrow-based function calls in the future
                    nodes.push_back(std::make_unique<RawJavaScriptNode>("->"));
                }
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

std::unique_ptr<ListenNode> CHTLJSParser::parseListenBlock() {
    advance(); // Consume 'Listen' identifier
    if (currentToken().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' after 'Listen'.");
    }
    advance(); // Consume '{'

    auto listenNode = std::make_unique<ListenNode>();

    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        if (currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) {
             throw std::runtime_error("Expected event name identifier in Listen block.");
        }
        std::string eventName = currentToken().value;
        advance();

        if (currentToken().type != CHTLJSTokenType::Colon) {
            throw std::runtime_error("Expected ':' after event name in Listen block.");
        }
        advance();

        if (currentToken().type != CHTLJSTokenType::RawJS) {
            throw std::runtime_error("Expected event handler function in Listen block.");
        }
        listenNode->event_handlers[eventName] = currentToken().value;
        advance();

        if (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }

    advance(); // Consume '}'
    return listenNode;
}