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
    // For now, we assume a single statement in the file.
    return parseStatement();
}

// --- Private Helper Methods ---

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (peek().type == TokenType::TEXT_KEYWORD) {
        return parseTextStatement();
    }
    if (peek().type == TokenType::IDENTIFIER) {
        return parseElementStatement();
    }
    // If no known statement pattern is found, advance and ignore.
    advance();
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextStatement() {
    advance(); // Consume 'text' keyword

    if (!match({TokenType::L_BRACE})) throw std::runtime_error("Expected '{' after 'text'.");

    Token content = advance();
    if (content.type != TokenType::STRING_LITERAL) throw std::runtime_error("Expected string literal inside text block.");

    if (!match({TokenType::R_BRACE})) throw std::runtime_error("Expected '}' after text block content.");

    return std::make_unique<TextNode>(content.value);
}

std::unique_ptr<BaseNode> CHTLParser::parseElementStatement() {
    Token identifier = advance(); // Consume identifier

    if (!match({TokenType::L_BRACE})) throw std::runtime_error("Expected '{' after element identifier.");

    auto element = std::make_unique<ElementNode>(identifier.value);

    // Recursively parse child nodes until a '}' is found.
    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        auto child = parseStatement();
        if (child) { // Add the child node if it's not null
            element->addChild(std::move(child));
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