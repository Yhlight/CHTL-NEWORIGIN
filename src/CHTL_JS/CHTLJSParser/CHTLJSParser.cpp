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
        case CHTLJSTokenType::OpenDoubleBrace:
             if (peekToken().type == CHTLJSTokenType::Identifier && tokens.size() > position + 3) {
                 if (tokens[position + 2].type == CHTLJSTokenType::CloseDoubleBrace) {
                     if(tokens[position + 3].type == CHTLJSTokenType::RightArrow && tokens.size() > position + 4) {
                         if(tokens[position + 4].type == CHTLJSTokenType::Listen) {
                            // Capture selector and pass it to parseListen
                            std::string selector = peekToken().value;
                            consumeToken(); // {{
                            consumeToken(); // selector
                            consumeToken(); // }}
                            return parseListen(selector);
                         }
                         if(tokens[position + 4].type == CHTLJSTokenType::Delegate) return parseDelegate();
                         return parseEnhancedSelector();
                     } else if (tokens[position + 3].type == CHTLJSTokenType::EventBind) {
                         return parseEventBinding();
                     }
                 }
            }
            return parseEnhancedSelector();
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
        consumeToken(); // ->
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

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegate() {
    // The dispatcher has identified {{...}}->Delegate, current token is `{{`
    expectToken(CHTLJSTokenType::OpenDoubleBrace);
    std::string parentSelector = currentToken().value;
    expectToken(CHTLJSTokenType::Identifier);
    expectToken(CHTLJSTokenType::CloseDoubleBrace);
    expectToken(CHTLJSTokenType::RightArrow);
    expectToken(CHTLJSTokenType::Delegate);
    expectToken(CHTLJSTokenType::OpenBrace);

    std::string targetSelector;
    std::map<std::string, std::string> events;

    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        if (key == "target") {
            targetSelector = currentToken().value;
            expectToken(CHTLJSTokenType::RawJavaScript);
        } else {
            std::string callback = currentToken().value;
            expectToken(CHTLJSTokenType::RawJavaScript);
            events[key] = callback;
        }

        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);

    if (targetSelector.empty()) {
        throw std::runtime_error("Delegate block must contain a 'target' property.");
    }

    return std::make_unique<DelegateNode>(parentSelector, targetSelector, events);
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimate() {
    expectToken(CHTLJSTokenType::Animate);
    expectToken(CHTLJSTokenType::OpenBrace);

    std::map<std::string, std::string> properties;
    while (currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        std::string value = currentToken().value;
        // The value can be a block or a single literal, so we treat it all as raw JS for now.
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

    std::string url;
    std::string page;

    while(currentToken().type != CHTLJSTokenType::CloseBrace) {
        std::string key = currentToken().value;
        expectToken(CHTLJSTokenType::Identifier);
        expectToken(CHTLJSTokenType::Colon);

        if (key == "url") {
            url = currentToken().value;
            expectToken(CHTLJSTokenType::RawJavaScript);
        } else if (key == "page") {
            page = currentToken().value;
            expectToken(CHTLJSTokenType::RawJavaScript);
        } else {
            throw std::runtime_error("Unexpected key in Router block: " + key);
        }

        if (currentToken().type == CHTLJSTokenType::Comma) {
            consumeToken();
        }
    }
    expectToken(CHTLJSTokenType::CloseBrace);

    if (url.empty() || page.empty()) {
        throw std::runtime_error("Router block must contain both 'url' and 'page' properties.");
    }

    // The URL value from the lexer will include quotes, e.g., `"/home"`. We need to strip them.
    if (url.length() >= 2 && url.front() == '"' && url.back() == '"') {
        url = url.substr(1, url.length() - 2);
    }

    std::map<std::string, std::string> routes;
    routes[url] = page;

    return std::make_unique<RouterNode>(routes);
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseEventBinding() {
    // The dispatcher has identified {{...}} &->, current token is `{{`
    expectToken(CHTLJSTokenType::OpenDoubleBrace);
    std::string selector = currentToken().value;
    expectToken(CHTLJSTokenType::Identifier);
    expectToken(CHTLJSTokenType::CloseDoubleBrace);
    expectToken(CHTLJSTokenType::EventBind);

    std::string eventName = currentToken().value;
    expectToken(CHTLJSTokenType::Identifier);
    expectToken(CHTLJSTokenType::Colon);

    std::string callback = currentToken().value;
    expectToken(CHTLJSTokenType::RawJavaScript);

    // Optional semicolon
    if (currentToken().type == CHTLJSTokenType::Semicolon) {
        consumeToken();
    }

    return std::make_unique<EventBindingNode>(selector, eventName, callback);
}