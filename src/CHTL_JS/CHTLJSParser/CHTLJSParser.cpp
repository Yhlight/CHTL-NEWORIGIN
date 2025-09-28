#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/VirtualObjectAccessNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include "../CHTLJSNode/ReservedPlaceholderNode.h"
#include <stdexcept>
#include <sstream>

// The constructor now accepts and stores the placeholder manager.
CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken> tokens, const Placeholder& placeholder_manager)
    : tokens(std::move(tokens)), placeholder_manager(&placeholder_manager) {}

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
        if (currentToken().value.rfind("_JS_CODE_PLACEHOLDER_", 0) == 0) {
            nodes.push_back(parseReservedPlaceholder());
            continue;
        }

        switch (currentToken().type) {
            case CHTLJSTokenType::Router:
                nodes.push_back(parseRouterBlock());
                break;
            case CHTLJSTokenType::Vir:
                nodes.push_back(parseVirtualObjectDeclaration());
                break;
            case CHTLJSTokenType::ScriptLoader:
                nodes.push_back(parseScriptLoaderBlock());
                break;
            case CHTLJSTokenType::OpenDoubleBrace: {
                advance();
                std::string selector_text = currentToken().value;
                // Resolve the placeholder to get the actual selector text.
                if (selector_text.rfind("_JS_CODE_PLACEHOLDER_", 0) == 0) {
                    selector_text = placeholder_manager->getContent(selector_text);
                }
                nodes.push_back(std::make_unique<CHTLJSEnhancedSelectorNode>(selector_text));
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) {
                    throw std::runtime_error("Unclosed enhanced selector.");
                }
                advance();
                break;
            }
            case CHTLJSTokenType::Arrow: {
                advance();
                if ((currentToken().type == CHTLJSTokenType::Identifier || currentToken().type == CHTLJSTokenType::RawJS) && currentToken().value == "Listen") {
                    nodes.push_back(parseListenBlock());
                } else if ((currentToken().type == CHTLJSTokenType::Identifier || currentToken().type == CHTLJSTokenType::RawJS) && currentToken().value == "Delegate") {
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
                if (position + 1 < tokens.size() && tokens[position + 1].type == CHTLJSTokenType::Arrow) {
                    std::string objectName = currentToken().value;
                    advance();
                    advance();
                    if (currentToken().type != CHTLJSTokenType::Identifier) {
                        throw std::runtime_error("Expected identifier after '->' for virtual object access.");
                    }
                    std::string propertyName = currentToken().value;
                    advance();
                    nodes.push_back(std::make_unique<VirtualObjectAccessNode>(objectName, propertyName));
                }
                else if (currentToken().value == "Animate") {
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

std::unique_ptr<ReservedPlaceholderNode> CHTLJSParser::parseReservedPlaceholder() {
    auto node = std::make_unique<ReservedPlaceholderNode>(currentToken().value);
    advance();
    return node;
}

std::unique_ptr<RouterNode> CHTLJSParser::parseRouterBlock() {
    advance();
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Router'.");
    advance();
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
                routerNode->routes[currentToken().value] = "";
                advance();
            } while (currentToken().type == CHTLJSTokenType::Comma);
        } else if (key == "page") {
            auto route_it = routerNode->routes.begin();
             do {
                if (currentToken().type == CHTLJSTokenType::Comma) advance();
                if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for router page selector.");
                advance();
                std::string page_selector = currentToken().value;
                if (page_selector.rfind("_JS_CODE_PLACEHOLDER_", 0) == 0) {
                    page_selector = placeholder_manager->getContent(page_selector);
                }
                if(route_it != routerNode->routes.end()) {
                    route_it->second = page_selector;
                    route_it++;
                }
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for router page selector.");
                advance();
            } while (currentToken().type == CHTLJSTokenType::Comma);
        } else if (key == "mode") {
            routerNode->mode = currentToken().value;
            advance();
        } else if (key == "root") {
            if (currentToken().type == CHTLJSTokenType::StringLiteral) {
                 routerNode->rootPath = currentToken().value;
                 advance();
            } else if (currentToken().type == CHTLJSTokenType::OpenDoubleBrace) {
                advance();
                 std::string root_selector = currentToken().value;
                if (root_selector.rfind("_JS_CODE_PLACEHOLDER_", 0) == 0) {
                    root_selector = placeholder_manager->getContent(root_selector);
                }
                routerNode->rootContainer = root_selector;
                advance();
                if (currentToken().type != CHTLJSTokenType::CloseDoubleBrace) throw std::runtime_error("Unclosed '{{...}}' for router root container.");
                advance();
            }
        }
        if(currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance();
    return routerNode;
}

std::unique_ptr<VirtualObjectNode> CHTLJSParser::parseVirtualObjectDeclaration() {
    advance();
    if (currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) {
        throw std::runtime_error("Expected identifier after 'Vir'.");
    }
    std::string name = currentToken().value;
    advance();
    if (currentToken().type != CHTLJSTokenType::Equals) throw std::runtime_error("Expected '=' after virtual object name.");
    advance();
    std::unique_ptr<CHTLJSNode> valueNode;
    if (currentToken().type == CHTLJSTokenType::Identifier) {
        if(currentToken().value == "Listen") valueNode = parseListenBlock();
        else if(currentToken().value == "Animate") valueNode = parseAnimateBlock();
        else if(currentToken().value == "Delegate") valueNode = parseDelegateBlock();
        else throw std::runtime_error("Unsupported function type for virtual object assignment.");
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
        if ((currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) || currentToken().value != "load") {
            throw std::runtime_error("Expected 'load' keyword in ScriptLoader block.");
        }
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after 'load' in ScriptLoader block.");
        advance();
        while (true) {
            if (currentToken().type == CHTLJSTokenType::CloseBrace) break;
            if (currentToken().type == CHTLJSTokenType::Identifier && currentToken().value == "load") break;
            scriptLoaderNode->paths.push_back(currentToken().value);
            advance();
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
    advance();
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
    advance();
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Listen'.");
    advance();
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
    advance();
    return listenNode;
}

std::unique_ptr<DelegateNode> CHTLJSParser::parseDelegateBlock() {
    advance();
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Delegate'.");
    advance();
    auto delegateNode = std::make_unique<DelegateNode>();
    auto parse_property = [&]() {
        if(currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) throw std::runtime_error("Expected property name in Delegate block.");
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after property name in Delegate block.");
        advance();
        if (key == "target") {
            auto parse_single_target = [&]() {
                if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for delegate target.");
                advance();
                std::string target_selector = currentToken().value;
                 if (target_selector.rfind("_JS_CODE_PLACEHOLDER_", 0) == 0) {
                    target_selector = placeholder_manager->getContent(target_selector);
                }
                delegateNode->target_selectors.push_back(target_selector);
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
            delegateNode->event_handlers[key] = parseJsCodeBlock();
        }
    };
    if (currentToken().type != CHTLJSTokenType::CloseBrace) {
        parse_property();
        while (currentToken().type == CHTLJSTokenType::Comma) {
            advance();
            if (currentToken().type == CHTLJSTokenType::CloseBrace) break;
            parse_property();
        }
    }
    if (currentToken().type != CHTLJSTokenType::CloseBrace) throw std::runtime_error("Unclosed Delegate block. Expected '}'.");
    advance();
    return delegateNode;
}

std::map<std::string, std::string> CHTLJSParser::parseStyleMap() {
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' to start a style map.");
    advance();
    std::map<std::string, std::string> styles;
    while(currentToken().type != CHTLJSTokenType::CloseBrace) {
        if (currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) throw std::runtime_error("Expected style property name.");
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after style property name.");
        advance();
        styles[key] = parseJsCodeBlock();
        if(currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance();
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
            if (currentToken().type != CHTLJSTokenType::Identifier && currentToken().type != CHTLJSTokenType::RawJS) throw std::runtime_error("Expected style property name in keyframe.");
            std::string key = currentToken().value;
            advance();
            if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after keyframe property.");
            advance();
            if (key == "at") {
                keyframe.at = std::stod(currentToken().value);
                advance();
            } else {
                keyframe.styles[key] = parseJsCodeBlock();
            }
            if(currentToken().type == CHTLJSTokenType::Comma) advance();
        }
        advance();
        keyframes.push_back(keyframe);
        if(currentToken().type == CHTLJSTokenType::Comma) advance();
    }
    advance();
    return keyframes;
}

std::unique_ptr<AnimateNode> CHTLJSParser::parseAnimateBlock() {
    advance();
    if (currentToken().type != CHTLJSTokenType::OpenBrace) throw std::runtime_error("Expected '{' after 'Animate'.");
    advance();
    auto animateNode = std::make_unique<AnimateNode>();
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        advance();
        if (currentToken().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after property name in Animate block.");
        advance();
        if (key == "target") {
            if (currentToken().type != CHTLJSTokenType::OpenDoubleBrace) throw std::runtime_error("Expected '{{' for animate target.");
            advance();
             std::string target_selector = currentToken().value;
            if (target_selector.rfind("_JS_CODE_PLACEHOLDER_", 0) == 0) {
                target_selector = placeholder_manager->getContent(target_selector);
            }
            animateNode->target = target_selector;
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
    advance();
    return animateNode;
}