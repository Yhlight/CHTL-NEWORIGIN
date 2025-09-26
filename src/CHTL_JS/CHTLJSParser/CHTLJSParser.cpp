#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
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
                } else if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "Delegate") {
                    nodes.push_back(parseDelegateBlock());
                } else {
                    nodes.push_back(std::make_unique<RawJavaScriptNode>("->"));
                }
                break;
            }
            case CHTLJSTokenType::EventBindingOperator: {
                nodes.push_back(parseEventBinding());
                break;
            }
            case CHTLJSTokenType::Identifier: {
                if (currentToken().value == "Animate") {
                    nodes.push_back(parseAnimateBlock());
                } else {
                    nodes.push_back(std::make_unique<RawJavaScriptNode>(currentToken().value));
                    advance();
                }
                break;
            }
            case CHTLJSTokenType::RawJS:
                nodes.push_back(std::make_unique<RawJavaScriptNode>(currentToken().value));
                advance();
                break;
            default:
                nodes.push_back(std::make_unique<RawJavaScriptNode>(currentToken().value));
                advance();
                break;
        }
    }

    return nodes;
}

std::unique_ptr<EventBindingNode> CHTLJSParser::parseEventBinding() {
    advance(); // Consume '&->'
    std::vector<std::string> event_names;
    while (currentToken().type == CHTLJSTokenType::Identifier || currentToken().type == CHTLJSTokenType::RawJS) {
        event_names.push_back(currentToken().value);
        advance();
        if (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
        } else {
            break;
        }
    }
    if (currentToken().type != CHTLJSTokenType::Colon) {
        throw std::runtime_error("Expected ':' after event name(s) in event binding.");
    }
    advance();
    if (currentToken().type != CHTLJSTokenType::RawJS) {
        throw std::runtime_error("Expected event handler function in event binding.");
    }
    std::string handler_code = currentToken().value;
    advance();
    return std::make_unique<EventBindingNode>(event_names, handler_code);
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListenBlock() {
    advance(); // Consume 'Listen'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' after 'Listen'.");
    }
    advance(); // Consume '{'
    auto listenNode = std::make_unique<ListenNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string eventName = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) {
            throw std::runtime_error("Expected ':' after event name in Listen block.");
        }
        advance();
        listenNode->event_handlers[eventName] = currentToken().value;
        advance();
        if (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }
    advance(); // Consume '}'
    return listenNode;
}

std::unique_ptr<DelegateNode> CHTLJSParser::parseDelegateBlock() {
    advance(); // Consume 'Delegate'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' after 'Delegate'.");
    }
    advance(); // Consume '{'
    auto delegateNode = std::make_unique<DelegateNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) {
            throw std::runtime_error("Expected ':' after property name in Delegate block.");
        }
        advance();
        if (key == "target") {
            // For now, we'll just read the raw JS for the target value.
            delegateNode->target_selectors.push_back(currentToken().value);
            advance();
        } else {
            delegateNode->event_handlers[key] = currentToken().value;
            advance();
        }
        if (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }
    advance(); // Consume '}'
    return delegateNode;
}

std::unique_ptr<AnimateNode> CHTLJSParser::parseAnimateBlock() {
    advance(); // Consume 'Animate'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' after 'Animate'.");
    }
    advance(); // Consume '{'
    auto animateNode = std::make_unique<AnimateNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) {
            throw std::runtime_error("Expected ':' after property name in Animate block.");
        }
        advance();
        if (key == "target") animateNode->target = currentToken().value;
        else if (key == "duration") animateNode->duration = std::stoi(currentToken().value);
        else if (key == "easing") animateNode->easing = currentToken().value;
        else if (key == "loop") animateNode->loop = std::stoi(currentToken().value);
        else if (key == "direction") animateNode->direction = currentToken().value;
        else if (key == "delay") animateNode->delay = std::stoi(currentToken().value);
        else if (key == "callback") animateNode->callback = currentToken().value;
        advance();
        if (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }
    advance(); // Consume '}'
    return animateNode;
}