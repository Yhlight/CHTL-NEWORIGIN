#include "CHTLJSParser.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include <stdexcept>

// Main entry point
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parse(const std::string& source) {
    CHTLJSLexer lexer(source);
    this->tokens = lexer.tokenize();
    this->position = 0;

    // For now, we assume a single statement per CHTL JS block
    return parseStatement();
}

// --- Token Management Helpers ---

CHTLJSToken CHTLJSParser::currentToken() {
    if (position >= tokens.size()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }
    return tokens[position];
}

CHTLJSToken CHTLJSParser::peekToken() {
    if (position + 1 >= tokens.size()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }
    return tokens[position + 1];
}

CHTLJSToken CHTLJSParser::consumeToken() {
    if (position >= tokens.size()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }
    return tokens[position++];
}

void CHTLJSParser::expectToken(CHTLJSTokenType type) {
    if (currentToken().type != type) {
        throw std::runtime_error("Unexpected token. Expected " + std::to_string((int)type) + " but got " + std::to_string((int)currentToken().type));
    }
    consumeToken();
}

// --- Main Parsing Logic ---

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    switch (currentToken().type) {
        case CHTLJSTokenType::Vir:
            return parseVir();
        case CHTLJSTokenType::Router:
            return parseRouter();
        case CHTLJSTokenType::Animate:
            return parseAnimate();
        case CHTLJSTokenType::ScriptLoader:
            return parseScriptLoader();
        case CHTLJSTokenType::OpenDoubleBrace: {
            consumeToken(); // {{
            std::string selector = currentToken().value;
            expectToken(CHTLJSTokenType::Identifier);
            expectToken(CHTLJSTokenType::CloseDoubleBrace); // }}

            if (currentToken().type == CHTLJSTokenType::RightArrow) {
                consumeToken(); // ->
                switch(currentToken().type) {
                    case CHTLJSTokenType::Listen:
                        return parseListen(selector);
                    case CHTLJSTokenType::Delegate:
                        return parseDelegate(selector);
                    default:
                        // Fallback to generic enhanced selector
                        position -= 4; // Backtrack to start of selector
                        return parseEnhancedSelector();
                }
            } else if (currentToken().type == CHTLJSTokenType::EventBind) {
                return parseEventBinding(selector);
            }
            // If it's just a selector, we might need to backtrack or handle it differently
            // For now, assume it's followed by an operator.
            position -= 3; // Backtrack
            return parseEnhancedSelector();
        }
        case CHTLJSTokenType::Listen: // Standalone Listen
            return parseListen("");
        default:
            return nullptr; // Or throw an error for unexpected token
    }
    return nullptr;
}

// --- Specific Construct Parsers ---

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseVir() {
    expectToken(CHTLJSTokenType::Vir);
    std::string name = currentToken().value;
    expectToken(CHTLJSTokenType::Identifier);
    expectToken(CHTLJSTokenType::Colon); // Using colon as assignment for Vir

    auto assignedNode = parseStatement();
    if (!assignedNode) {
        throw std::runtime_error("Expected a CHTL JS expression after 'Vir " + name + " :'");
    }
    return std::make_unique<VirtualObjectNode>(name, std::move(assignedNode));
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseListen(const std::string& selector_str) {
    std::string selector = selector_str;

    if (selector.empty()) { // Standalone Listen
        expectToken(CHTLJSTokenType::Listen);
    } else { // Chained from selector
        // consumeToken(); // -> is already consumed by the dispatcher
        expectToken(CHTLJSTokenType::Listen);
    }

    expectToken(CHTLJSTokenType::OpenBrace);

    std::map<std::string, std::string> events;
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string eventName = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        std::string callback = currentToken().value;
        expectToken(CHTLJSTokenType::RawJavaScript);

        events[eventName] = callback;

        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);
    return std::make_unique<ListenNode>(selector, events);
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnhancedSelector() {
    expectToken(CHTLJSTokenType::OpenDoubleBrace);
    std::string selector = currentToken().value;
    expectToken(CHTLJSTokenType::Identifier);
    expectToken(CHTLJSTokenType::CloseDoubleBrace);
    expectToken(CHTLJSTokenType::RightArrow);

    std::string expression = currentToken().value;
    expectToken(CHTLJSTokenType::RawJavaScript);

    return std::make_unique<EnhancedSelectorNode>(selector, expression);
}


std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseScriptLoader() {
    expectToken(CHTLJSTokenType::ScriptLoader);
    expectToken(CHTLJSTokenType::OpenBrace);

    std::vector<std::string> files;
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        expectToken(CHTLJSTokenType::Load);
        expectToken(CHTLJSTokenType::Colon);
        files.push_back(currentToken().value);
        expectToken(CHTLJSTokenType::String);
        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);
    return std::make_unique<ScriptLoaderNode>(files);
}

// Stubs for other parsers - to be implemented
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegate(const std::string& parent_selector) {
    expectToken(CHTLJSTokenType::Delegate);
    expectToken(CHTLJSTokenType::OpenBrace);

    std::string target_selector;
    std::map<std::string, std::string> events;

    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        if (key == "target") {
            target_selector = currentToken().value;
            // The spec says the target can be {{selector}} or [{{s1}}, {{s2}}...]
            // This simplified parser will just grab the raw JS part for now.
            expectToken(CHTLJSTokenType::RawJavaScript);
        } else {
            // Assume it's an event handler
            std::string callback = currentToken().value;
            expectToken(CHTLJSTokenType::RawJavaScript);
            events[key] = callback;
        }

        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);

    if (target_selector.empty()) {
        throw std::runtime_error("Delegate block must contain a 'target' property.");
    }

    return std::make_unique<DelegateNode>(parent_selector, target_selector, events);
}
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimate() {
    expectToken(CHTLJSTokenType::Animate);
    expectToken(CHTLJSTokenType::OpenBrace);

    std::map<std::string, std::string> properties;
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        // The lexer will provide the complex value (e.g., array, object for 'when')
        // as a single raw token. This simplifies the parser logic significantly.
        std::string value = currentToken().value;
        expectToken(CHTLJSTokenType::RawJavaScript);

        properties[key] = value;

        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);

    return std::make_unique<AnimateNode>(properties);
}
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseRouter() {
    expectToken(CHTLJSTokenType::Router);
    expectToken(CHTLJSTokenType::OpenBrace);

    std::map<std::string, std::string> routes;
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        std::string value = currentToken().value;
        expectToken(CHTLJSTokenType::RawJavaScript);

        routes[key] = value;

        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);

    return std::make_unique<RouterNode>(routes);
}
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseEventBinding(const std::string& selector) {
    expectToken(CHTLJSTokenType::EventBind); // &->

    // The spec allows for multiple events, comma-separated. This simplified
    // parser will handle a single event for now.
    std::string event_name = currentToken().value;
    expectToken(CHTLJSTokenType::Identifier);
    expectToken(CHTLJSTokenType::Colon);

    std::string callback = currentToken().value;
    expectToken(CHTLJSTokenType::RawJavaScript);

    return std::make_unique<EventBindingNode>(selector, event_name, callback);
}