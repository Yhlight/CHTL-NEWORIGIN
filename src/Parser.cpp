#include "Parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<ElementNode> Parser::parse() {
    // For now, we assume the root is a single element.
    // A more robust implementation might handle multiple root-level nodes.
    if (peek().type == TokenType::Identifier) {
        return parse_element();
    }
    return nullptr;
}

std::unique_ptr<BaseNode> Parser::parse_node() {
    if (peek().type != TokenType::Identifier) {
        // Should not happen in valid CHTL for now
        return nullptr;
    }

    if (peek().value == "text") {
        advance(); // consume 'text'
        if (match(TokenType::LeftBrace)) {
            return parse_text_block();
        }
        // text: "value"; is handled inside parse_element_content as an attribute
    } else if (peek().value == "style") {
        advance(); // consume 'style'
        consume(TokenType::LeftBrace, "Expect '{' after 'style' keyword.");
        return parse_style_block();
    }

    return parse_element();
}

std::unique_ptr<ElementNode> Parser::parse_element() {
    Token tag_token = consume(TokenType::Identifier, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tag_token.value);

    consume(TokenType::LeftBrace, "Expect '{' after element tag name.");

    while (!check(TokenType::RightBrace) && !is_at_end()) {
        parse_element_content(element.get());
    }

    consume(TokenType::RightBrace, "Expect '}' to close element.");
    return element;
}

void Parser::parse_element_content(ElementNode* element) {
    if (peek().type != TokenType::Identifier) {
        // Error or unexpected token
        advance();
        return;
    }

    // Check for attribute syntax: identifier : value ; OR identifier = value ;
    if (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals) {
        std::string key = advance().value;
        advance(); // consume ':' or '='

        if (key == "text") {
             Token value_token = consume(TokenType::StringLiteral, "Expect string literal for text attribute.");
             element->add_child(std::make_unique<TextNode>(value_token.value));
        } else {
            Token value_token = advance(); // StringLiteral or Identifier (for unquoted)
            element->add_attribute(std::make_unique<AttributeNode>(key, value_token.value));
        }
        consume(TokenType::Semicolon, "Expect ';' after attribute value.");

    } else { // It must be a nested element or other node type
        element->add_child(parse_node());
    }
}


std::unique_ptr<TextNode> Parser::parse_text_block() {
    // Assumes we have just consumed "text {"
    Token content = consume(TokenType::StringLiteral, "Expect string literal inside text block.");
    consume(TokenType::RightBrace, "Expect '}' to close text block.");
    return std::make_unique<TextNode>(content.value);
}

std::unique_ptr<StyleNode> Parser::parse_style_block() {
    // For now, we are not parsing the CSS content. We just find the matching braces.
    // A more advanced parser would tokenize and parse the CSS.
    std::string content;
    int brace_level = 1;
    while(brace_level > 0 && !is_at_end()) {
        if (peek().type == TokenType::LeftBrace) brace_level++;
        if (peek().type == TokenType::RightBrace) brace_level--;

        if (brace_level > 0) {
            content += advance().value + " ";
        } else {
            consume(TokenType::RightBrace, "Expect '}' to close style block.");
        }
    }
    return std::make_unique<StyleNode>(content);
}


// Helper methods
Token& Parser::peek() { return tokens[current]; }
Token& Parser::previous() { return tokens[current - 1]; }
bool Parser::is_at_end() { return peek().type == TokenType::EndOfFile; }

Token& Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    // In a real compiler, this would throw a proper error.
    std::cerr << "Parse Error: " << message << " at line " << peek().line << std::endl;
    exit(1); // Simple error handling
}
