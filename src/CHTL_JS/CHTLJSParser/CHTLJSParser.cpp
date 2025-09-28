#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/VirtualObjectAccessNode.h"
#include "../CHTLJSNode/RouterNode.h"
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

std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSNode>> nodes;
    while (currentToken().type != CHTLJSTokenType::EndOfFile) {
        nodes.push_back(parseNode());
    }
    return nodes;
}

std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSParser::parseBlock(CHTLJSTokenType closing_token) {
    std::vector<std::unique_ptr<CHTLJSNode>> nodes;
    while (currentToken().type != closing_token && currentToken().type != CHTLJSTokenType::EndOfFile) {
        nodes.push_back(parseNode());
    }
    return nodes;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseNode() {
    const auto& token = currentToken();
    switch (token.type) {
        case CHTLJSTokenType::Placeholder:
        case CHTLJSTokenType::ReservedPlaceholder: {
            auto node = std::make_unique<CHTLJSPlaceholderNode>(token.value);
            advance();
            return node;
        }
        case CHTLJSTokenType::Router:
            return parseRouterBlock();
        case CHTLJSTokenType::Vir:
            return parseVirtualObjectDeclaration();
        case CHTLJSTokenType::ScriptLoader:
            return parseScriptLoaderBlock();
        case CHTLJSTokenType::OpenDoubleBrace: {
            advance();
            if (currentToken().type != CHTLJSTokenType::Identifier) {
                throw std::runtime_error("Expected identifier inside enhanced selector.");
            }
            auto node = std::make_unique<CHTLJSEnhancedSelectorNode>(currentToken().value);
            advance();
            if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) {
                throw std::runtime_error("Unclosed enhanced selector.");
            }
            advance();
            return node;
        }
        case CHTLJSTokenType::Arrow: {
             advance();
             if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "Listen") {
                return parseListenBlock();
             } else if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "Delegate") {
                return parseDelegateBlock();
             } else {
                throw std::runtime_error("Unexpected token after '->'");
             }
        }
        case CHTLJSTokenType::EventBindingOperator: {
            auto node = parseEventBinding();
            if (currentToken().type == CHTLJSTokenType::Semicolon) {
                advance();
            }
            return node;
        }
        case CHTLJSTokenType::Identifier: {
             if (position + 1 < tokens.size() && tokens[position + 1].type == CHTLJSTokenType::Arrow) {
                std::string objectName = currentToken().value;
                advance();
                advance();
                if (currentToken().type != CHTLJSTokenType::Identifier) {
                    throw std::runtime_error("Expected identifier after '->' for virtual object access.");
                }
                std::string propertyName = currentToken().value;
                advance();
                return std::make_unique<VirtualObjectAccessNode>(objectName, propertyName);
            }
            else if (currentToken().value == "Animate") {
                return parseAnimateBlock();
            } else {
                throw std::runtime_error("Unexpected identifier: " + currentToken().value);
            }
        }
        default:
            throw std::runtime_error("Unexpected token: " + token.value);
    }
}

std::unique_ptr<RouterNode> CHTLJSParser::parseRouterBlock() {
    advance(); // Consume 'Router'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Router'.");
    advance(); // Consume '{'

    auto routerNode = std::make_unique<RouterNode>();

    while(currentToken().type != CHTLJSTokenType::CloseBrace) {
        if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected identifier in Router block.");
        std::string key = currentToken().value;
        advance();

        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after router key.");
        advance();

        if (key == "url") {
            do {
                if (currentToken().type == CHTLJSTokenType::Comma) advance();
                if (currentToken().type != CHTLJSTokenType::StringLiteral) throw std::runtime_error("Expected string literal for router URL.");
                routerNode->routes[currentToken().value] = ""; // Placeholder for page
                advance();
            } while (currentToken().type == CHTLJSTokenType::Comma);
        } else if (key == "page") {
            auto route_it = routerNode->routes.begin();
             do {
                if (currentToken().type == CHTLJSTokenType::Comma) advance();
                if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for router page selector.");
                advance();
                if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected selector inside '{{...}}'.");

                if(route_it != routerNode->routes.end()) {
                    route_it->second = currentToken().value;
                    route_it++;
                }
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for router page selector.");
                advance();
            } while (currentToken().type == CHTLJSTokenType::Comma);
        } else if (key == "mode") {
            if (currentToken().type != CHTLJSTokenType::StringLiteral) throw std::runtime_error("Expected string literal for router mode.");
            routerNode->mode = currentToken().value;
            advance();
        } else if (key == "root") {
            if (currentToken().type == CHTLJSTokenType::StringLiteral) {
                 routerNode->rootPath = currentToken().value;
                 advance();
            } else if (currentToken().type == CHTLJSTokenType::OpenDoubleBrace) {
                advance();
                if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected selector inside '{{...}}'.");
                routerNode->rootContainer = currentToken().value;
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for router root container.");
                advance();
            }
        }

        if(currentToken().type == CHTLJSTokenType::Comma) advance();
    }

    advance(); // Consume '}'
    return routerNode;
}

std::unique_ptr<VirtualObjectNode> CHTLJSParser::parseVirtualObjectDeclaration() {
    advance(); // Consume 'Vir'

    if (currentToken().type != CHTLJSTokenType::Identifier) {
        throw std::runtime_error("Expected identifier after 'Vir'.");
    }
    std::string name = currentToken().value;
    advance();

    if (currentToken().type != CHTLJSTokenType::Equals) {
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
        if (currentToken().type != CHTLJSTokenType::Identifier || currentToken().value != "load") {
            throw std::runtime_error("Expected 'load' keyword in ScriptLoader block.");
        }
        advance();

        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after 'load' in ScriptLoader block.");
        advance();

        while (true) {
            if (currentToken().type == CHTLJSTokenType::CloseBrace) break;
            if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "load") break;

            if (currentToken().type == CHTLJSTokenType::StringLiteral || currentToken().type == CHTLJSTokenType::Identifier) {
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
    while (currentToken().type == CHTLJSTokenType::Identifier) {
        event_names.push_back(currentToken().value);
        advance();
        if (currentToken().type == CHTLJSTokenType::Comma) advance();
        else break;
    }
    if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after event name(s) in event binding.");
    advance();

    std::vector<std::unique_ptr<CHTLJSNode>> handler_body;
    while (currentToken().type != CHTLJSTokenType::Semicolon && currentToken().type != CHTLJSTokenType::EndOfFile) {
        handler_body.push_back(parseNode());
    }

    return std::make_unique<EventBindingNode>(event_names, std::move(handler_body));
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListenBlock() {
    advance(); // Consume 'Listen'
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Listen'.");
    advance(); // Consume '{'
    auto listenNode = std::make_unique<ListenNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        if (currentToken().type != CHTLJSTokenType::Identifier) {
            throw std::runtime_error("Expected event name identifier in Listen block.");
        }
        std::string eventName = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after event name in Listen block.");
        advance();

        std::vector<std::unique_ptr<CHTLJSNode>> handler_body;
        while (currentToken().type != CHTLJSTokenType::Comma && currentToken().type != CHTLJSTokenType::CloseBrace) {
            handler_body.push_back(parseNode());
        }
        listenNode->event_handlers[eventName] = std::move(handler_body);

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

    auto parse_property = [&]() {
        if(currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected property name in Delegate block.");
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after property name in Delegate block.");
        advance();

        if (key == "target") {
            auto parse_single_target = [&]() {
                if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for delegate target.");
                advance();
                if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected selector inside '{{...}}'.");
                delegateNode->target_selectors.push_back(currentToken().value);
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for delegate target.");
                advance();
            };

            if (currentToken().type == CHTLJSTokenType::OpenBracket) {
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseBracket) {
                    parse_single_target();
                    while (currentToken().type == CHTLJSTokenType::Comma) {
                        advance();
                        parse_single_target();
                    }
                }
                if (currentToken().type != CHTLJSTokenType::CloseBracket) throw std::runtime_error("Unclosed array ']' in delegate target.");
                advance();
            } else {
                parse_single_target();
            }
        } else {
            std::vector<std::unique_ptr<CHTLJSNode>> handler_body;
            while (currentToken().type != CHTLJSTokenType::Comma && currentToken().type != CHTLJSTokenType::CloseBrace) {
                handler_body.push_back(parseNode());
            }
            delegateNode->event_handlers[key] = std::move(handler_body);
        }
    };

    if (currentToken().type != CHTLJSTokenType::CloseBrace) {
        parse_property();
        while (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
            if (currentToken().type == CHTLJSTokenType::CloseBrace) break; // Allow trailing comma
            parse_property();
        }
    }

    if (currentToken().type != CHTLJSTokenType::CloseBrace) throw std::runtime_error("Unclosed Delegate block. Expected '}'.");
    advance(); // Consume '}'
    return delegateNode;
}

std::map<std::string, std::string> CHTLJSParser::parseStyleMap() {
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' to start a style map.");
    advance();
    std::map<std::string, std::string> styles;
    while(currentToken().type != CHTLJSTokenType::CloseBrace) {
        if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected style property name.");
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after style property name.");
        advance();
        if (currentToken().type != CHTLJSTokenType::ReservedPlaceholder && currentToken().type != CHTLJSTokenType::StringLiteral && currentToken().type != CHTLJSTokenType::Identifier) {
             throw std::runtime_error("Expected a placeholder, string, or identifier for the style value.");
        }
        styles[key] = currentToken().value;
        advance();
        if(currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance(); // Consume '}'
    return styles;
}

std::vector<AnimationKeyframe> CHTLJSParser::parseKeyframeArray() {
    if (currentToken().type != CHTLJSTokenType::OpenBracket) throw std::runtime_error("Expected '[' to start a keyframe array.");
    advance();
    std::vector<AnimationKeyframe> keyframes;
    while(currentToken().type != CHTLJSTokenType::CloseBracket) {
        if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' for keyframe definition.");
        advance();
        AnimationKeyframe keyframe;
        while(currentToken().type != CHTLJSTokenType::CloseBrace) {
            if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected style property name in keyframe.");
            std::string key = currentToken().value;
            advance();
            if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after keyframe property.");
            advance();
            if (key == "at") {
                keyframe.at = std::stod(currentToken().value);
                advance();
            } else {
                 if (currentToken().type != CHTLJSTokenType::ReservedPlaceholder && currentToken().type != CHTLJSTokenType::StringLiteral && currentToken().type != CHTLJSTokenType::Identifier) {
                    throw std::runtime_error("Expected a placeholder, string, or identifier for the style value.");
                 }
                keyframe.styles[key] = currentToken().value;
                advance();
            }
            if(currentToken().type == CHTLJSTokenType::Comma) advance();
        }
        advance(); // Consume '}'
        keyframes.push_back(keyframe);
        if(currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance(); // Consume ']'
    return keyframes;
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
            if (currentToken().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected selector inside '{{...}}'.");
            animateNode->target = currentToken().value;
            advance();
            if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for animate target.");
            advance();
        } else if (key == "begin") {
            animateNode->begin_styles = parseStyleMap();
        } else if (key == "end") {
            animateNode->end_styles = parseStyleMap();
        } else if (key == "when") {
            animateNode->when_keyframes = parseKeyframeArray();
        } else {
            if (currentToken().type != CHTLJSTokenType::ReservedPlaceholder && currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::StringLiteral) {
                 throw std::runtime_error("Expected a placeholder, identifier, or string for animate property value.");
            }
            if (key == "callback") {
                std::vector<std::unique_ptr<CHTLJSNode>> callback_body;
                while (currentToken().type != CHTLJSTokenType::Comma && currentToken().type != CHTLJSTokenType::CloseBrace) {
                    callback_body.push_back(parseNode());
                }
                animateNode->callback = std::move(callback_body);
            } else {
                std::string value = currentToken().value;
                advance();
                if (key == "duration") animateNode->duration = std::stoi(value);
                else if (key == "easing") animateNode->easing = value;
                else if (key == "loop") animateNode->loop = std::stoi(value);
                else if (key == "direction") animateNode->direction = value;
                else if (key == "delay") animateNode->delay = std::stoi(value);
            }
        }

        if (currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance(); // Consume '}'
    return animateNode;
}