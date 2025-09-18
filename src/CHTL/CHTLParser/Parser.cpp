#include "Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <iostream>
#include <string>
#include <vector>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

NodePtr Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!isAtEnd()) {
        try {
            NodePtr child = declaration();
            if (child) {
                root->addChild(std::move(child));
            }
        } catch (const ParseError& error) {
            synchronize();
        }
    }
    return root;
}

// --- Error Handling & Consumption ---

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else if (token.type != TokenType::UNKNOWN) {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
    return ParseError(message);
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

// --- Grammar Rule Implementations ---

NodePtr Parser::declaration() {
    if (check(TokenType::IDENTIFIER) && (peekNext().type == TokenType::COLON || peekNext().type == TokenType::EQUALS)) {
        throw error(peek(), "Attributes can only be defined inside an element block.");
    } else if (match({TokenType::KEYWORD_TEXT})) {
        return textNode();
    } else if (match({TokenType::GENERATOR_COMMENT})) {
        return std::make_unique<CommentNode>(previous().lexeme, CommentType::Generator);
    } else if (match({TokenType::IDENTIFIER})) {
        return element();
    }

    throw error(peek(), "Expect a declaration (e.g., an element, text block, or generator comment).");
}

NodePtr Parser::element() {
    Token tagName = previous();
    auto node = std::make_unique<ElementNode>(tagName.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Ignore non-generator comments
        if (match({TokenType::LINE_COMMENT, TokenType::BLOCK_COMMENT})) {
            continue;
        }

        if (check(TokenType::IDENTIFIER) && (peekNext().type == TokenType::COLON || peekNext().type == TokenType::EQUALS)) {
            parseAttribute(*node);
        } else {
            NodePtr child = declaration();
            if (child) {
                node->addChild(std::move(child));
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");

    return node;
}

void Parser::parseAttribute(ElementNode& owner) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute name.");

    if (!match({TokenType::COLON, TokenType::EQUALS})) {
        throw error(peek(), "Expect ':' or '=' after attribute name.");
    }

    Token value;
    if (match({TokenType::IDENTIFIER, TokenType::STRING_LITERAL, TokenType::NUMBER_LITERAL})) {
        value = previous();
    } else {
        throw error(peek(), "Expect attribute value.");
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");

    owner.addAttribute(std::make_unique<AttributeNode>(key.lexeme, value.lexeme));
}

NodePtr Parser::textNode() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text' keyword.");

    std::string content_str;
    if (check(TokenType::STRING_LITERAL)) {
        content_str = consume(TokenType::STRING_LITERAL, "").lexeme;
    } else {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            content_str += advance().lexeme;
            // Add a space if the next token isn't punctuation or the end of the block.
            TokenType next_type = peek().type;
            if (next_type != TokenType::RIGHT_BRACE && next_type != TokenType::DOT && next_type != TokenType::COMMA && next_type != TokenType::SEMICOLON) {
                 content_str += " ";
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after text content.");

    return std::make_unique<TextNode>(content_str);
}

// --- Token Stream Helpers ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) {
        return tokens.back(); // Should be EOF
    }
    return tokens[current + 1];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::KEYWORD_TEMPLATE:
            case TokenType::KEYWORD_CUSTOM:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace CHTL
