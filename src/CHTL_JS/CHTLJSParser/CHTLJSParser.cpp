#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include <stdexcept>
#include <sstream>

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

std::string CHTLJSParser::parseJsCodeBlock() {
    std::stringstream code_stream;
    int brace_level = 0;

    while (currentToken().type != CHTLJSTokenType::EndOfFile) {
        const auto& token = currentToken();

        if (brace_level == 0 && (token.type == CHTLJSTokenType::Comma || token.type == CHTLJSTokenType::CloseBrace || token.type == CHTLJSTokenType::Semicolon)) {
            break;
        }

        if (token.type == CHTLJSTokenType::OpenBrace) brace_level++;
        else if (token.type == CHTLJSTokenType::CloseBrace) brace_level--;

        code_stream << token.value;
        advance();
    }
    return code_stream.str();
}


std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSNode>> nodes;

    while (currentToken().type != CHTLJSTokenType::EndOfFile) {
        switch (currentToken().type) {
            case CHTLJSTokenType::Vir:
                nodes.push_back(parseVirtualObjectDeclaration());
                break;
            case CHTLJSTokenType::ScriptLoader:
                nodes.push_back(parseScriptLoaderBlock());
                break;
            case CHTLJSTokenType::OpenDoubleBrace: {
                advance();
                if (currentToken().type != CHTLJSTokenType::RawJS) {
                    throw std::runtime_error("Expected raw text inside enhanced selector.");
                }
                nodes.push_back(std::make_unique<CHTLJSEnhancedSelectorNode>(currentToken().value));
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) {
                    throw std::runtime_error("Unclosed enhanced selector.");
                }
                advance();
                break;
            }
            case CHTLJSTokenType::Arrow: {
                advance();
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
                if (currentToken().type == CHTLJSTokenType::Semicolon) {
                    advance();
                }
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
            default:
                nodes.push_back(std::make_unique<RawJavaScriptNode>(currentToken().value));
                advance();
                break;
        }
    }

    return nodes;
}

std::unique_ptr<VirtualObjectNode> CHTLJSParser::parseVirtualObjectDeclaration() {
    advance(); // Consume 'Vir'

    if (currentToken().type != CHTLJSTokenType::Identifier) {
        throw std::runtime_error("Expected identifier after 'Vir'.");
    }
    std::string name = currentToken().value;
    advance();

    if (currentToken().type != CHTLJSTokenType::RawJS || currentToken().value != "=") {
        throw std::runtime_error("Expected '=' after virtual object name.");
    }
    advance();

    std::unique_ptr<CHTLJSNode> valueNode;
    if (currentToken().type == CHTLJSTokenType::Identifier) {
        if(currentToken().value == "Listen") valueNode = parseListenBlock();
        else if(currentToken().value == "Animate") valueNode = parseAnimateBlock();
        else if(currentToken().value == "Delegate") valueNode = parseDelegateBlock();
        else {
            throw std::runtime_error("Unsupported function type for virtual object assignment.");
        }
    } else {
        throw std::runtime_error("Expected a CHTL JS function after '=' in virtual object declaration.");
    }

    return std::make_unique<VirtualObjectNode>(name, std::move(valueNode));
}

std::unique_ptr<ScriptLoaderNode> CHTLJSParser::parseScriptLoaderBlock() {
    advance();
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'ScriptLoader'.");
    advance();

    auto scriptLoaderNode = std::make_unique<ScriptLoaderNode>();

    while (currentToken().type != CHTLJSTokenType::CloseBrace && currentToken().type != CHTLJSTokenType::EndOfFile) {
        if (currentToken().type != CHTLJSTokenType::Identifier || currentToken().value != "load") throw std::runtime_error("Expected 'load' keyword in ScriptLoader block.");
        advance();

        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after 'load' in ScriptLoader block.");
        advance();

        while (true) {
            if (currentToken().type == CHTLJSTokenType::CloseBrace) break;
            if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "load") break;

            if (currentToken().type == CHTLJSTokenType::StringLiteral || currentToken().type == CHTLJSTokenType::RawJS || currentToken().type == CHTLJSTokenType::Identifier) {
                scriptLoaderNode->paths.push_back(currentToken().value);
                advance();
            } else {
                throw std::runtime_error("Expected a file path in ScriptLoader block.");
            }

            if (currentToken().type == CHTLJSTokenType::Comma) advance();
            else if (currentToken().type == CHTLJSTokenType::Semicolon) { advance(); break; }
            else break;
        }
    }

    if (currentToken().type != CHTLJSTokenType::CloseBrace) throw std::runtime_error("Unclosed ScriptLoader block.");
    advance();

    return scriptLoaderNode;
}

std::unique_ptr<EventBindingNode> CHTLJSParser::parseEventBinding() {
    advance(); // Consume '&->'
    std::vector<std::string> event_names;
    while (currentToken().type == CHTLJSTokenType::Identifier || currentToken().type == CHTLJSTokenType::RawJS) {
        event_names.push_back(currentToken().value);
        advance();
        if (currentToken().type == CHTLJSTokenType::Comma) advance();
        else break;
    }
    if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after event name(s) in event binding.");
    advance();

    std::string handler_code = parseJsCodeBlock();

    return std::make_unique<EventBindingNode>(event_names, handler_code);
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListenBlock() {
    advance(); // Consume 'Listen'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Listen'.");
    advance(); // Consume '{'
    auto listenNode = std::make_unique<ListenNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        if (currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) {
            throw std::runtime_error("Expected event name identifier in Listen block.");
        }
        std::string eventName = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after event name in Listen block.");
        advance();

        listenNode->event_handlers[eventName] = parseJsCodeBlock();

        if (currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance(); // Consume '}'
    return listenNode;
}

std::unique_ptr<DelegateNode> CHTLJSParser::parseDelegateBlock() {
    advance(); // Consume 'Delegate'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Delegate'.");
    advance(); // Consume '{'
    auto delegateNode = std::make_unique<DelegateNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after property name in Delegate block.");
        advance();
        if (key == "target") {
            if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for delegate target.");
            advance();
            if (currentToken().type != CHTLJSTokenType::RawJS) throw std::runtime_error("Expected selector inside '{{...}}'.");
            delegateNode->target_selectors.push_back(currentToken().value);
            advance();
            if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for delegate target.");
            advance();
        } else {
            delegateNode->event_handlers[key] = parseJsCodeBlock();
        }
        if (currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance(); // Consume '}'
    return delegateNode;
}

std::unique_ptr<AnimateNode> CHTLJSParser::parseAnimateBlock() {
    advance(); // Consume 'Animate'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Animate'.");
    advance(); // Consume '{'
    auto animateNode = std::make_unique<AnimateNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after property name in Animate block.");
        advance();

        if (key == "target") {
            if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for animate target.");
            advance();
            if (currentToken().type != CHTLJSTokenType::RawJS) throw std::runtime_error("Expected selector inside '{{...}}'.");
            animateNode->target = currentToken().value;
            advance();
            if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for animate target.");
            advance();
        } else {
            std::string value = parseJsCodeBlock();
            if (key == "duration") animateNode->duration = std::stoi(value);
            else if (key == "easing") animateNode->easing = value;
            else if (key == "loop") animateNode->loop = std::stoi(value);
            else if (key == "direction") animateNode->direction = value;
            else if (key == "delay") animateNode->delay = std::stoi(value);
            else if (key == "callback") animateNode->callback = value;
        }

        if (currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance(); // Consume '}'
    return animateNode;
}