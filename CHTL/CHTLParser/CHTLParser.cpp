#include "CHTLParser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/ElementNode.h"
#include <stdexcept> // For std::runtime_error

namespace CHTL {

// --- Public Methods ---

CHTLParser::CHTLParser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

CHTLParser::~CHTLParser() {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }
    return parseStatement();
}

// --- Private Helper Methods ---

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (peek().type == TokenType::TEXT_KEYWORD) {
        return parseTextStatement();
    }
    if (peek().type == TokenType::IDENTIFIER) {
        // This could be an element or an attribute. Attributes are only valid
        // inside an element block, so we assume this is an element.
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
    Token key = advance(); // Consume identifier (key)

    if (!match({TokenType::COLON, TokenType::EQUAL})) throw std::runtime_error("Expected ':' or '=' after attribute key.");

    Token value = advance(); // Consume value
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
        // Look ahead to see if it's an attribute (IDENTIFIER : or IDENTIFIER =)
        if (peek().type == TokenType::IDENTIFIER && current + 1 < tokens.size() &&
            (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
            parseAttributeStatement(*element);
        } else {
            // Otherwise, it's a nested statement (element, text, etc.)
            auto child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            }
        }
    }

    if (!match({TokenType::R_BRACE})) throw std::runtime_error("Expected '}' after element block.");

    return element;
}


// --- Token Stream Management ---

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

} // namespace CHTL