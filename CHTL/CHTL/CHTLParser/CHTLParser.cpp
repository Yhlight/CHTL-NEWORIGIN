#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLLexer/Token.h" // For TokenType
#include <vector>

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<CHTLNode> CHTLParser::parse() {
    // For now, we assume the input is a single element declaration.
    // In the future, this would parse a list of top-level declarations.
    if (!isAtEnd()) {
        return declaration();
    }
    return nullptr;
}

#include "../CHTLNode/CommentNode.h"

std::unique_ptr<CHTLNode> CHTLParser::declaration() {
    if (match({TokenType::KW_TEXT})) return textDeclaration();
    if (check(TokenType::IDENTIFIER)) return elementDeclaration();
    if (match({TokenType::GENERATOR_COMMENT})) {
        return std::make_unique<CommentNode>(previous().literal);
    }

    // In the future, handle other declaration types.
    // For now, if it's not text or an element, it's an error.
    throw error(peek(), "Expect a declaration.");
}

std::unique_ptr<CHTLNode> CHTLParser::elementDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect element name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");
    std::vector<std::unique_ptr<CHTLNode>> children = block();
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return std::make_unique<ElementNode>(name.lexeme, std::move(children));
}

std::unique_ptr<CHTLNode> CHTLParser::textDeclaration() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text' keyword.");
    Token content = consume(TokenType::STRING_LITERAL, "Expect string literal inside text block.");
    // For now, we assume a single string literal followed by an optional semicolon.
    match({TokenType::SEMICOLON});
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    // The literal field now contains the clean string content.
    return std::make_unique<TextNode>(content.literal);
}

std::vector<std::unique_ptr<CHTLNode>> CHTLParser::block() {
    std::vector<std::unique_ptr<CHTLNode>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token CHTLParser::peek() {
    return tokens[current];
}

Token CHTLParser::previous() {
    return tokens[current - 1];
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

CHTLParser::ParseError CHTLParser::error(const Token& token, const std::string& message) {
    // In a real compiler, you'd have a more sophisticated error reporter.
    // For now, we'll just throw an exception.
    std::string error_message = "Parse Error at line " + std::to_string(token.line) + ": " + message;
    return ParseError(error_message);
}
