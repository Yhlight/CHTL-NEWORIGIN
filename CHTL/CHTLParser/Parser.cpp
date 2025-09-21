#include "Parser.h"

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// ---- Main Parsing Logic ----

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        // Skip comments at the top level
        while (match(TokenType::COMMENT)) {}
        if (isAtEnd()) break;

        program->children.push_back(parseStatement());
    }
    return program;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (peek().type == TokenType::IDENTIFIER) {
        if (peek().lexeme == "text") {
            return parseTextNode();
        }
        // It's an element by default
        return parseElement();
    }

    // If we are here, it's an unexpected token.
    // A real implementation would have better error recovery.
    // For now, we advance and return nullptr.
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = advance().lexeme; // Consume the identifier

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Skip any comments inside the element body
        while (match(TokenType::COMMENT)) {}
        if (check(TokenType::RIGHT_BRACE)) break;

        // Check if it's an attribute (identifier followed by : or =)
        if (peek().type == TokenType::IDENTIFIER && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
             parseAttributes(*element);
        } else {
            // Otherwise, it must be a child node
            element->children.push_back(parseStatement());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return element;
}

void Parser::parseAttributes(ElementNode& element) {
    std::string key = advance().lexeme; // Consume attribute key

    // Consume ':' or '='
    if (!match(TokenType::COLON) && !match(TokenType::EQUAL)) {
        throw ParseError("Expect ':' or '=' after attribute name.");
    }

    // Value can be an identifier (unquoted) or a string
    if (!check(TokenType::IDENTIFIER) && !check(TokenType::STRING)) {
        throw ParseError("Expect attribute value (identifier or string).");
    }
    Token value = advance();

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");

    element.attributes.push_back({key, value});
}


std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto textNode = std::make_unique<TextNode>();
    consume(TokenType::IDENTIFIER, "Expect 'text' keyword."); // Consume 'text'
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");

    if (!check(TokenType::IDENTIFIER) && !check(TokenType::STRING)) {
        throw ParseError("Expect text content (identifier or string).");
    }
    textNode->content = advance();

    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return textNode;
}


// ---- Utility and Helper Methods ----

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    throw ParseError(message);
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

const Token& Parser::peek() const {
    return tokens[current];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::TOKEN_EOF;
}

} // namespace CHTL
