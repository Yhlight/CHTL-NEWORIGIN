#include "Parser.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

ParseResult Parser::parse() {
    ParseResult result;
    while (!is_at_end()) {
        try {
            NodePtr node = parse_node(result.global_styles);
            if (node) {
                result.ast.push_back(std::move(node));
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse Error at line " << peek().line << ": " << e.what() << std::endl;
            break;
        }
    }
    return result;
}

NodePtr Parser::parse_node(CssRuleList& global_rules) {
    if (peek().type == TokenType::IDENTIFIER) {
        return parse_element(global_rules);
    }
    // ... (rest of parse_node is the same for now)
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

NodePtr Parser::parse_element(CssRuleList& global_rules) {
    Token tag_token = consume(TokenType::IDENTIFIER, "Expected element tag name.");

    consume(TokenType::LEFT_BRACE, "Expected '{' after element tag name.");

    // We need to create the node earlier to pass it to parse_style_block
    auto element_node = std::make_unique<ElementNode>(tag_token.lexeme, AttributeList{}, StylePropertyList{}, NodeList{});

    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        if (peek().type == TokenType::IDENTIFIER) {
            if (tokens.size() > current + 1 && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                element_node->attributes.push_back(parse_attribute());
            } else if (peek().lexeme == "style") {
                parse_style_block(element_node->styles, global_rules, *element_node);
            } else {
                element_node->children.push_back(parse_element(global_rules));
            }
        } else if (peek().type == TokenType::TEXT_KEYWORD) {
            // ... (same text block logic as before)
             if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::LEFT_BRACE) {
                element_node->children.push_back(parse_text_block());
            } else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
                advance(); // consume 'text'
                advance(); // consume ':'
                Token value = consume(TokenType::STRING, "Expected string after 'text:'.");
                consume(TokenType::SEMICOLON, "Expected ';' after text attribute.");
                element_node->children.push_back(std::make_unique<TextNode>(value.lexeme));
            } else {
                throw std::runtime_error("Invalid syntax for 'text'. Expected '{' or ':'.");
            }
        } else if (peek().type == TokenType::HASH_COMMENT) {
            // ... (same hash comment logic)
            Token comment_token = advance();
            std::string content = comment_token.lexeme;
            size_t first_char = content.find_first_not_of("# \t");
            if (first_char != std::string::npos) {
                content = content.substr(first_char);
            }
            element_node->children.push_back(std::make_unique<CommentNode>(content));
        } else {
            throw std::runtime_error("Unexpected token in element body: " + peek().lexeme);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close element.");
    return element_node;
}

std::string Parser::parse_selector() {
    std::string selector;
    while(peek().type == TokenType::DOT || peek().type == TokenType::HASH || peek().type == TokenType::AMPERSAND || peek().type == TokenType::IDENTIFIER || peek().type == TokenType::COLON) {
        if(peek().type == TokenType::COLON) {
             selector += advance().lexeme;
             if(peek().type == TokenType::COLON) { // handle ::
                 selector += advance().lexeme;
             }
        } else {
             selector += advance().lexeme;
        }
    }
    return selector;
}

void Parser::parse_style_block(StylePropertyList& styles, CssRuleList& global_rules, ElementNode& parent_element) {
    consume(TokenType::IDENTIFIER, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'.");

    std::string current_context_selector;

    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        if (peek().type == TokenType::DOT || peek().type == TokenType::HASH || peek().type == TokenType::AMPERSAND) {
            // This is a nested CSS rule
            CssRule rule;
            rule.selector = parse_selector();

            consume(TokenType::LEFT_BRACE, "Expected '{' after selector.");
            while(!check(TokenType::RIGHT_BRACE)) {
                rule.properties.push_back(parse_style_property());
            }
            consume(TokenType::RIGHT_BRACE, "Expected '}' after rule block.");

            if (rule.selector[0] == '&') {
                rule.context_selector = current_context_selector;
            } else if (rule.selector[0] == '.') {
                std::string class_name = rule.selector.substr(1);
                auto it = std::find_if(parent_element.attributes.begin(), parent_element.attributes.end(), [](const Attribute& attr) {
                    return attr.key == "class";
                });
                if (it != parent_element.attributes.end()) {
                    it->value += " " + class_name;
                } else {
                    parent_element.attributes.push_back({"class", class_name});
                }
                current_context_selector = rule.selector;
                rule.context_selector = current_context_selector;
            } else if (rule.selector[0] == '#') {
                 std::string id_name = rule.selector.substr(1);
                 parent_element.attributes.push_back({"id", id_name});
                 current_context_selector = rule.selector;
                 rule.context_selector = current_context_selector;
            }

            global_rules.push_back(rule);
        } else {
            // This is an inline style property
            styles.push_back(parse_style_property());
        }
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
    while(peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && !is_at_end()) {
        value_str += advance().lexeme;
    }

    if (value_str.empty()) {
        throw std::runtime_error("Expected style property value.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
    // trim leading space
    value_str.erase(0, value_str.find_first_not_of(" \t\n\r"));
    return {key.lexeme, value_str};
}

NodePtr Parser::parse_text_block() {
    consume(TokenType::TEXT_KEYWORD, "Expected 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'.");

    std::string text_content;
    if (peek().type == TokenType::STRING) {
        text_content = advance().lexeme;
    } else {
        while (peek().type != TokenType::RIGHT_BRACE && !is_at_end()) {
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
