#include "Parser.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "CHTL/Expression/LiteralExpr.h"
#include "CHTL/Expression/UnaryExpr.h"
#include "CHTL/Expression/BinaryExpr.h"
#include "CHTL/Expression/LogicalExpr.h"
#include "CHTL/Expression/ConditionalExpr.h"
#include "CHTL/Expression/ReferenceExpr.h"


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

void Parser::parse_style_block(StylePropertyList& styles, CssRuleList& global_rules, ElementNode& parent_element) {
    consume(TokenType::IDENTIFIER, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'.");
    std::string current_context_selector;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        if (peek().type == TokenType::DOT || peek().type == TokenType::HASH || peek().type == TokenType::AMPERSAND) {
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
                auto it = std::find_if(parent_element.attributes.begin(), parent_element.attributes.end(), [](const Attribute& attr) { return attr.key == "class"; });
                if (it != parent_element.attributes.end()) { it->value += " " + class_name; }
                else { parent_element.attributes.push_back({"class", class_name}); }
                current_context_selector = rule.selector;
                rule.context_selector = current_context_selector;
            } else if (rule.selector[0] == '#') {
                 std::string id_name = rule.selector.substr(1);
                 parent_element.attributes.push_back({"id", id_name});
                 current_context_selector = rule.selector;
                 rule.context_selector = current_context_selector;
            }
            global_rules.push_back(std::move(rule));
        } else {
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
    if (peek().type == TokenType::STRING) { value = advance(); }
    else if (peek().type == TokenType::IDENTIFIER) { value = advance(); }
    else { throw std::runtime_error("Expected attribute value."); }
    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
    return {key.lexeme, value.lexeme};
}

StyleProperty Parser::parse_style_property() {
    Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
    consume(TokenType::COLON, "Expected ':' after style property name.");
    ExprPtr value = parse_expression();
    consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
    return {key.lexeme, std::move(value)};
}

std::string Parser::parse_selector() {
    std::string selector;
    while(peek().type == TokenType::DOT || peek().type == TokenType::HASH || peek().type == TokenType::AMPERSAND || peek().type == TokenType::IDENTIFIER || peek().type == TokenType::COLON) {
        if(peek().type == TokenType::COLON) {
             selector += advance().lexeme;
             if(peek().type == TokenType::COLON) { selector += advance().lexeme; }
        } else {
             selector += advance().lexeme;
        }
    }
    return selector;
}

// --- Expression Parsing ---

ExprPtr Parser::parse_expression() {
    return parse_conditional();
}

ExprPtr Parser::parse_conditional() {
    ExprPtr expr = parse_logical_or();
    if (match({TokenType::QUESTION})) {
        ExprPtr then_branch = parse_expression();
        consume(TokenType::COLON, "Expect ':' after then branch in conditional expression.");
        ExprPtr else_branch = parse_conditional();
        expr = std::make_unique<ConditionalExpr>(std::move(expr), std::move(then_branch), std::move(else_branch));
    }
    return expr;
}

ExprPtr Parser::parse_logical_or() {
    ExprPtr expr = parse_logical_and();
    while (match({TokenType::LOGICAL_OR})) {
        Token op = previous();
        ExprPtr right = parse_logical_and();
        expr = std::make_unique<LogicalExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parse_logical_and() {
    ExprPtr expr = parse_equality();
    while (match({TokenType::LOGICAL_AND})) {
        Token op = previous();
        ExprPtr right = parse_equality();
        expr = std::make_unique<LogicalExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parse_equality() {
    ExprPtr expr = parse_comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        ExprPtr right = parse_comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parse_comparison() {
    ExprPtr expr = parse_term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        ExprPtr right = parse_term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parse_term() {
    ExprPtr expr = parse_factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        ExprPtr right = parse_factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parse_factor() {
    ExprPtr expr = parse_unary();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        ExprPtr right = parse_unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parse_unary() {
    if (match({TokenType::MINUS})) {
        Token op = previous();
        ExprPtr right = parse_unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return parse_primary();
}

ExprPtr Parser::parse_primary() {
    if (peek().type == TokenType::DOT || peek().type == TokenType::HASH || peek().type == TokenType::IDENTIFIER) {
        return parse_reference();
    }

    if (peek().type == TokenType::STRING) {
        return std::make_unique<LiteralExpr>(advance().lexeme);
    }

    if (match({TokenType::LEFT_PAREN})) {
        ExprPtr expr = parse_expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }

    if (peek().type == TokenType::NUMBER) {
        std::string value = advance().lexeme;
        // Also consume a unit if it's there (e.g., 'px', 'em')
        if (peek().type == TokenType::IDENTIFIER) {
            value += advance().lexeme;
        }
        return std::make_unique<LiteralExpr>(value);
    }

    throw std::runtime_error("Expected expression.");
}

ExprPtr Parser::parse_reference() {
    std::vector<Token> path;
    // A reference can be a single identifier, or a path like .box.width
    if (peek().type == TokenType::DOT || peek().type == TokenType::HASH) {
        path.push_back(advance()); // consume . or #
    }

    // Then consume a chain of identifiers and dots
    while (true) {
        if (peek().type == TokenType::IDENTIFIER) {
            path.push_back(advance());
        } else {
            // After a . or #, an identifier must follow.
            if (path.size() == 1 && (path[0].type == TokenType::DOT || path[0].type == TokenType::HASH)) {
                 throw std::runtime_error("Expected identifier after '.' or '#'.");
            }
        }

        if (match({TokenType::DOT})) {
            path.push_back(previous());
        } else {
            break;
        }
    }

    if (path.empty()) {
        throw std::runtime_error("Invalid reference expression.");
    }
    return std::make_unique<ReferenceExpr>(path);
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
bool Parser::is_at_end() const { return tokens[current].type == TokenType::END_OF_FILE; }
Token Parser::advance() { if (!is_at_end()) current++; return previous(); }
Token Parser::peek() const { return tokens[current]; }
Token Parser::previous() const { return tokens[current - 1]; }
bool Parser::check(TokenType type) const { if (is_at_end()) return false; return peek().type == type; }
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
