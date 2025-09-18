#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<ElementNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "root";

    while (!isAtEnd()) {
        root->children.push_back(parseDeclaration());
    }

    return root;
}

std::unique_ptr<BaseNode> Parser::parseDeclaration() {
    if (match(TokenType::KEYWORD_TEXT)) {
        return parseTextNode();
    }
    if (match(TokenType::UNQUOTED_LITERAL)) {
        // This is an element, put the token back to be consumed by parseElement
        current--;
        return parseElement();
    }
     if (check(TokenType::SINGLE_LINE_COMMENT) || check(TokenType::MULTI_LINE_COMMENT) || check(TokenType::GENERATOR_COMMENT)) {
        return parseCommentNode(peek().type);
    }


    // If we are at the end, it's not an error, just return nullptr.
    if (isAtEnd() || check(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }

    throw std::runtime_error("Unexpected token at top level: " + peek().value);
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto node = std::make_unique<TextNode>();
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text' keyword.");

    if(check(TokenType::STRING_LITERAL) || check(TokenType::UNQUOTED_LITERAL)) {
        node->content = advance().value;
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return node;
}

std::unique_ptr<CommentNode> Parser::parseCommentNode(TokenType type) {
    auto node = std::make_unique<CommentNode>();
    node->content = advance().value;
    if (type == TokenType::SINGLE_LINE_COMMENT) node->type = CommentType::SINGLE_LINE;
    else if (type == TokenType::MULTI_LINE_COMMENT) node->type = CommentType::MULTI_LINE;
    else node->type = CommentType::GENERATOR;
    return node;
}


std::unique_ptr<ElementNode> Parser::parseElement() {
    auto node = std::make_unique<ElementNode>();

    if (check(TokenType::UNQUOTED_LITERAL)) {
         node->tagName = advance().value;
    } else {
        throw std::runtime_error("Expected element tag name.");
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Check for attributes
        if (peek().type == TokenType::UNQUOTED_LITERAL && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS)) {
            parseAttributes(*node);
        } else {
            auto child = parseDeclaration();
            if (child) {
                node->children.push_back(std::move(child));
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

void Parser::parseAttributes(ElementNode& node) {
    std::string key = consume(TokenType::UNQUOTED_LITERAL, "Expect attribute name.").value;

    if (!match(TokenType::COLON) && !match(TokenType::EQUALS)) {
        throw std::runtime_error("Expect ':' or '=' after attribute name.");
    }

    if (!check(TokenType::STRING_LITERAL) && !check(TokenType::UNQUOTED_LITERAL)) {
        throw std::runtime_error("Expect attribute value.");
    }
    std::string value = advance().value;

    node.attributes[key] = value;

    // Consume optional semicolon
    match(TokenType::SEMICOLON);
}


// --- Token Helpers ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " Found " + peek().value + " instead.");
}

} // namespace CHTL
