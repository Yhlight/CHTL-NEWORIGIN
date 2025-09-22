#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

NodeList Parser::parse() {
    NodeList nodes;
    while (!is_at_end()) {
        try {
            NodePtr node = parse_node();
            if (node) { // parse_node can return nullptr for EOF
                nodes.push_back(std::move(node));
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse Error at line " << peek().line << ": " << e.what() << std::endl;
            // For now, we stop at the first error. A more robust parser could synchronize.
            break;
        }
    }
    return nodes;
}

NodePtr Parser::parse_node() {
    if (peek().type == TokenType::IDENTIFIER) {
        return parse_element();
    }
    if (peek().type == TokenType::TEXT_KEYWORD) {
        return parse_text_block();
    }
    if (peek().type == TokenType::HASH_COMMENT) {
        Token comment_token = advance();
        std::string content = comment_token.lexeme;
        size_t first_char = content.find_first_not_of("# \t");
        if (first_char != std::string::npos) {
            content = content.substr(first_char);
        }
        return std::make_unique<CommentNode>(content);
    }
    if (peek().type == TokenType::END_OF_FILE) {
        return nullptr;
    }
    throw std::runtime_error("Unexpected token: " + peek().lexeme);
}

NodePtr Parser::parse_element() {
    Token tag_token = consume(TokenType::IDENTIFIER, "Expected element tag name.");
    std::string tag_name = tag_token.lexeme;

    consume(TokenType::LEFT_BRACE, "Expected '{' after element tag name.");

    AttributeList attributes;
    StylePropertyList styles;
    NodeList children;

    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        if (peek().type == TokenType::IDENTIFIER) {
            if (tokens.size() > current + 1 && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                attributes.push_back(parse_attribute());
            } else if (peek().lexeme == "style") {
                parse_style_block(styles);
            } else {
                children.push_back(parse_element());
            }
        } else if (peek().type == TokenType::TEXT_KEYWORD) {
            if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::LEFT_BRACE) {
                children.push_back(parse_text_block());
            } else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
                advance(); // consume 'text'
                advance(); // consume ':'
                Token value = consume(TokenType::STRING, "Expected string after 'text:'.");
                consume(TokenType::SEMICOLON, "Expected ';' after text attribute.");
                children.push_back(std::make_unique<TextNode>(value.lexeme));
            } else {
                throw std::runtime_error("Invalid syntax for 'text'. Expected '{' or ':'.");
            }
        } else if (peek().type == TokenType::HASH_COMMENT) {
            Token comment_token = advance();
            std::string content = comment_token.lexeme;
            size_t first_char = content.find_first_not_of("# \t");
            if (first_char != std::string::npos) {
                content = content.substr(first_char);
            }
            children.push_back(std::make_unique<CommentNode>(content));
        } else {
            throw std::runtime_error("Unexpected token in element body: " + peek().lexeme);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close element.");

    return std::make_unique<ElementNode>(tag_name, std::move(attributes), std::move(styles), std::move(children));
}

void Parser::parse_style_block(StylePropertyList& styles) {
    consume(TokenType::IDENTIFIER, "Expected 'style' keyword."); // Expect 'style'
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'.");
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        styles.push_back(parse_style_property());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block.");
}

Attribute Parser::parse_attribute() {
    Token key = consume(TokenType::IDENTIFIER, "Expected attribute name.");
    if (!match({TokenType::COLON, TokenType::EQUAL})) {
        throw std::runtime_error("Expected ':' or '=' after attribute name.");
    }

    Token value;
    if (peek().type == TokenType::STRING) {
        value = advance();
    } else if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::UNQUOTED_LITERAL) {
        value = advance();
    } else {
        throw std::runtime_error("Expected attribute value.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
    return {key.lexeme, value.lexeme};
}

StyleProperty Parser::parse_style_property() {
    Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
    consume(TokenType::COLON, "Expected ':' after style property name.");

    std::string value_str;
    while(peek().type == TokenType::IDENTIFIER || peek().type == TokenType::UNQUOTED_LITERAL || peek().type == TokenType::STRING) {
        if (!value_str.empty()) value_str += " ";
        value_str += advance().lexeme;
    }

    if (value_str.empty()) {
        throw std::runtime_error("Expected style property value.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
    return {key.lexeme, value_str};
}

NodePtr Parser::parse_text_block() {
    consume(TokenType::TEXT_KEYWORD, "Expected 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'.");

    std::string text_content;
    if (peek().type == TokenType::STRING) {
        text_content = advance().lexeme;
    } else {
        // Handle multi-word unquoted literals.
        while (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::UNQUOTED_LITERAL) {
            if (!text_content.empty()) {
                text_content += " ";
            }
            text_content += advance().lexeme;
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after text block.");
    return std::make_unique<TextNode>(text_content);
}

// --- Token stream management ---

bool Parser::is_at_end() const {
    return tokens[current].type == TokenType::END_OF_FILE;
}

Token Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
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

Token Parser::consume(TokenType type, const std::string& error_message) {
    if (check(type)) return advance();
    throw std::runtime_error(error_message + " Got '" + peek().lexeme + "' instead.");
}
