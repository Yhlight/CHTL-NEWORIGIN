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
            if (peekToken().type == CHTLJSTokenType::Identifier) {
                 if (tokens[position + 2].type == CHTLJSTokenType::CloseDoubleBrace) {
                     if(tokens[position + 3].type == CHTLJSTokenType::RightArrow) {
                         // This could be Listen, Delegate, or Enhanced Selector
                         if(tokens[position + 4].type == CHTLJSTokenType::Listen) return parseListen("");
                         if(tokens[position + 4].type == CHTLJSTokenType::Delegate) return parseDelegate();
                         return parseEnhancedSelector();
                     } else if (tokens[position + 3].type == CHTLJSTokenType::EventBind) {
                         return parseEventBinding();
                     }
                 }
            }
            break;
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

// Stubs for other parsers - to be implemented
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegate() {
    // To be implemented
    // For now, consume tokens to avoid infinite loops in testing
    while(currentToken().type != CHTLJSTokenType::EndOfFile) consumeToken();
    return nullptr;
}
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimate() {
    // To be implemented
    while(currentToken().type != CHTLJSTokenType::EndOfFile) consumeToken();
    return nullptr;
}
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseRouter() {
    // To be implemented
    while(currentToken().type != CHTLJSTokenType::EndOfFile) consumeToken();
    return nullptr;
}
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseEventBinding() {
    // To be implemented
    while(currentToken().type != CHTLJSTokenType::EndOfFile) consumeToken();
    return nullptr;
}