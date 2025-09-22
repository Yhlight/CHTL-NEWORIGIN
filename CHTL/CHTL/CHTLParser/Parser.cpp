#include "Parser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLExpression/ExpressionParser.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<BaseNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!isAtEnd()) {
        root->children.push_back(parseElement());
    }
    return root;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

// For now, a simple error recovery
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (tokens[current - 1].type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::KEYWORD_TEXT:
            case TokenType::IDENTIFIER: // Assuming new element starts
                return;
            default:
                break;
        }
        advance();
    }
}


std::unique_ptr<ElementNode> Parser::parseElement() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(identifier.lexeme);

    if (match({TokenType::LEFT_BRACE})) {
        parseBlock(element.get());
    }

    return element;
}

void Parser::parseBlock(ElementNode* element) {
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::KEYWORD_TEXT) {
            // Handle text: "value"; or text { "value" }
            advance(); // consume 'text'
            if (match({TokenType::COLON, TokenType::EQUALS})) {
                 Token value = consume(TokenType::STRING_LITERAL, "Expect string literal after 'text:'.");
                 element->children.push_back(std::make_unique<TextNode>(value.lexeme));
                 consume(TokenType::SEMICOLON, "Expect ';' after text attribute.");
            } else if (match({TokenType::LEFT_BRACE})) {
                Token value = consume(TokenType::STRING_LITERAL, "Expect string literal inside text block.");
                element->children.push_back(std::make_unique<TextNode>(value.lexeme));
                consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
            }
        } else if (peek().type == TokenType::IDENTIFIER && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS)) {
            parseAttribute(element);
        } else if (peek().type == TokenType::KEYWORD_STYLE) {
            element->children.push_back(parseStyleBlock());
        } else if (peek().type == TokenType::IDENTIFIER) {
            element->children.push_back(parseElement());
        } else {
            // Should not happen on happy path
            advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
}

std::unique_ptr<StyleBlockNode> Parser::parseStyleBlock() {
    consume(TokenType::KEYWORD_STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");

    auto styleNode = std::make_unique<StyleBlockNode>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Check if it's a property (IDENTIFIER... :) or a nested rule (IDENTIFIER... {)
        bool is_property = false;
        if (current + 1 < tokens.size()) {
            if (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS) {
                is_property = true;
            }
        }

        // This lookahead is still too simple. A key can be multiple tokens.
        // A better lookahead is to scan until a `{` or a `:`.
        int temp_pos = current;
        while(temp_pos < tokens.size() && tokens[temp_pos].type != TokenType::LEFT_BRACE && tokens[temp_pos].type != TokenType::COLON && tokens[temp_pos].type != TokenType::EQUALS) {
            temp_pos++;
        }

        if (temp_pos < tokens.size() && (tokens[temp_pos].type == TokenType::COLON || tokens[temp_pos].type == TokenType::EQUALS)) {
             // It's an inline property
            std::stringstream key_stream;
            while (!check(TokenType::COLON) && !check(TokenType::EQUALS) && !isAtEnd()) {
                key_stream << advance().lexeme;
            }
            std::string key = key_stream.str();

            if (!match({TokenType::COLON, TokenType::EQUALS})) {
                throw std::runtime_error("Expect ':' or '=' after css property name.");
            }

            std::vector<Token> value_tokens;
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                value_tokens.push_back(advance());
            }
            ExpressionParser expr_parser(value_tokens);
            std::unique_ptr<ExpressionNode> value_expr = expr_parser.parse();

            consume(TokenType::SEMICOLON, "Expect ';' after css property value.");
            styleNode->properties.push_back(std::make_unique<CssPropertyNode>(key, std::move(value_expr)));
        } else {
            // It's a nested rule
            styleNode->rules.push_back(parseCssRule());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return styleNode;
}

std::unique_ptr<CssRuleNode> Parser::parseCssRule() {
    // For now, selector is a simple concatenation of tokens until '{'
    std::stringstream selector_stream;
    while(!check(TokenType::LEFT_BRACE) && !isAtEnd()){
        selector_stream << advance().lexeme;
    }
    auto ruleNode = std::make_unique<CssRuleNode>(selector_stream.str());

    consume(TokenType::LEFT_BRACE, "Expect '{' after selector.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        std::stringstream key_stream;
        while (!check(TokenType::COLON) && !check(TokenType::EQUALS) && !isAtEnd()) {
            key_stream << advance().lexeme;
        }
        std::string key = key_stream.str();

        if (!match({TokenType::COLON, TokenType::EQUALS})) {
            throw std::runtime_error("Expect ':' or '=' after css property name.");
        }

        std::vector<Token> value_tokens;
        while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
            value_tokens.push_back(advance());
        }
        ExpressionParser expr_parser(value_tokens);
        std::unique_ptr<ExpressionNode> value_expr = expr_parser.parse();

        consume(TokenType::SEMICOLON, "Expect ';' after css property value.");
        ruleNode->properties.push_back(std::make_unique<CssPropertyNode>(key, std::move(value_expr)));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after CSS rule block.");
    return ruleNode;
}

void Parser::parseAttribute(ElementNode* element) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    if (!match({TokenType::COLON, TokenType::EQUALS})) {
        throw std::runtime_error("Expect ':' or '=' after attribute name.");
    }

    std::string final_value;
    if (peek().type == TokenType::STRING_LITERAL) {
        final_value = consume(TokenType::STRING_LITERAL, "Expect string literal.").lexeme;
    } else {
        std::stringstream value_stream;
        while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
            value_stream << advance().lexeme;
        }
        final_value = value_stream.str();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    element->attributes.push_back(std::make_unique<AttributeNode>(key.lexeme, final_value));
}

} // namespace CHTL
