#include "CHTLParser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

CHTLParser::~CHTLParser() {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }
    return parseStatement();
}

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (peek().type == TokenType::TEXT_KEYWORD) {
        return parseTextStatement();
    }
    if (peek().type == TokenType::IDENTIFIER) {
        return parseElementStatement();
    }
    advance();
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextStatement() {
    advance();
    if (!match({TokenType::L_BRACE})) throw std::runtime_error("Expected '{' after 'text'.");
    Token content = advance();
    if (content.type != TokenType::STRING_LITERAL) throw std::runtime_error("Expected string literal inside text block.");
    if (!match({TokenType::R_BRACE})) throw std::runtime_error("Expected '}' after text block content.");
    return std::make_unique<TextNode>(content.value);
}

void CHTLParser::parseAttributeStatement(ElementNode& owner) {
    Token key = advance();
    if (!match({TokenType::COLON, TokenType::EQUAL})) throw std::runtime_error("Expected ':' or '=' after attribute key.");
    Token value = advance();
    if (value.type != TokenType::STRING_LITERAL && value.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected string literal or identifier for attribute value.");
    }
    if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after attribute value.");
    owner.addAttribute(key.value, value.value);
}

std::unique_ptr<BaseNode> CHTLParser::parseElementStatement() {
    Token identifier = advance();
    if (!match({TokenType::L_BRACE})) throw std::runtime_error("Expected '{' after element identifier.");
    auto element = std::make_unique<ElementNode>(identifier.value);
    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::STYLE_KEYWORD) {
            element->setStyle(parseStyleStatement());
        } else if (peek().type == TokenType::SCRIPT_KEYWORD) {
            element->setScript(parseScriptStatement());
        } else if (peek().type == TokenType::IDENTIFIER && current + 1 < tokens.size() && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
            parseAttributeStatement(*element);
        } else {
            auto child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            }
        }
    }
    if (!match({TokenType::R_BRACE})) throw std::runtime_error("Expected '}' after element block.");
    return element;
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleStatement() {
    advance();
    if (!match({TokenType::L_BRACE})) throw std::runtime_error("Expected '{' after 'style' keyword.");
    auto styleNode = std::make_unique<StyleNode>();
    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        Token key = advance();
        if (key.type != TokenType::IDENTIFIER) throw std::runtime_error("Expected style property key.");
        if (!match({TokenType::COLON})) throw std::runtime_error("Expected ':' after style property key.");
        std::string combined_value;
        while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
            combined_value += advance().value;
        }
        if (combined_value.empty()) {
            throw std::runtime_error("Expected a value for the style property.");
        }
        if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after style property.");
        styleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, combined_value));
    }
    if (!match({TokenType::R_BRACE})) throw std::runtime_error("Expected '}' to close style block.");
    return styleNode;
}

std::unique_ptr<ScriptNode> CHTLParser::parseScriptStatement() {
    advance(); // Consume 'script' keyword
    if (!match({TokenType::L_BRACE})) throw std::runtime_error("Expected '{' after 'script' keyword.");

    Token content = peek();
    if (content.type == TokenType::STRING_LITERAL) {
        advance(); // Consume content
    } else {
        // Handle empty script block
        content = Token{TokenType::STRING_LITERAL, ""};
    }

    if (!match({TokenType::R_BRACE})) throw std::runtime_error("Expected '}' to close script block.");
    return std::make_unique<ScriptNode>(content.value);
}

bool CHTLParser::isAtEnd() {
    return current >= tokens.size();
}

Token CHTLParser::peek() {
    if (isAtEnd()) return Token{TokenType::END_OF_FILE, ""};
    return tokens[current];
}

Token CHTLParser::previous() {
    return tokens[current - 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

}