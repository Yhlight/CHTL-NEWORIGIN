#include "ExpressionParser.h"
#include <stdexcept>

ExpressionParser::ExpressionParser(std::vector<ValueToken> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<ExprNode> ExpressionParser::parse() {
    return parse_expression();
}

// expression -> term ( ( "+" | "-" ) term )*
std::unique_ptr<ExprNode> ExpressionParser::parse_expression() {
    auto node = parse_term();

    while (peek().type == ValueTokenType::Operator && (peek().value == "+" || peek().value == "-")) {
        char op = advance().value[0];
        auto right = parse_term();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
    }

    return node;
}

// term -> factor ( ( "*" | "/" ) factor )*
std::unique_ptr<ExprNode> ExpressionParser::parse_term() {
    auto node = parse_factor();

    while (peek().type == ValueTokenType::Operator && (peek().value == "*" || peek().value == "/")) {
        char op = advance().value[0];
        auto right = parse_factor();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
    }

    return node;
}

// factor -> primary ( "px" | "em" | etc. )?
std::unique_ptr<ExprNode> ExpressionParser::parse_factor() {
    return parse_primary();
}

// primary -> NUMBER [IDENTIFIER] | SELECTOR "." IDENTIFIER
std::unique_ptr<ExprNode> ExpressionParser::parse_primary() {
    if (peek().type == ValueTokenType::Number) {
        double value = std::stod(advance().value);
        std::string unit;
        if (peek().type == ValueTokenType::Identifier) {
            unit = advance().value;
        }
        return std::make_unique<NumberNode>(value, unit);
    }

    if (peek().type == ValueTokenType::Selector) {
        std::string selector = advance().value;
        if (tokens[current].type != ValueTokenType::Dot) {
             throw std::runtime_error("Expression parser: Expected '.' after selector.");
        }
        advance(); // consume dot

        if (tokens[current].type != ValueTokenType::Identifier) {
            throw std::runtime_error("Expression parser: Expected property name after selector.");
        }
        std::string prop_name = advance().value;

        return std::make_unique<PropertyReferenceNode>(selector, prop_name);
    }

    throw std::runtime_error("Expression parser: Unexpected token.");
}

ValueToken& ExpressionParser::peek() {
    return tokens[current];
}

ValueToken& ExpressionParser::advance() {
    if (!is_at_end()) {
        current++;
    }
    return tokens[current - 1];
}

bool ExpressionParser::is_at_end() {
    return current >= tokens.size() || tokens[current].type == ValueTokenType::EndOfValue;
}
