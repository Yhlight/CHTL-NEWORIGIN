#include "ExpressionParser.h"
#include <stdexcept>

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token ExpressionParser::current_token() {
    if (pos >= tokens.size()) {
        return {TokenType::EndOfFile, "", -1, -1};
    }
    return tokens[pos];
}

void ExpressionParser::consume_token() {
    pos++;
}

int ExpressionParser::get_token_precedence() {
    switch (current_token().type) {
        case TokenType::Less:
        case TokenType::Greater:
        case TokenType::LessEqual:
        case TokenType::GreaterEqual:
        case TokenType::EqualEqual:
        case TokenType::NotEqual:
            return 10;
        case TokenType::Plus:
        case TokenType::Minus:
            return 20;
        case TokenType::Star:
        case TokenType::Slash:
            return 40;
        default:
            return -1;
    }
}

ExprNodePtr ExpressionParser::parse_primary() {
    Token token = current_token();
    consume_token();
    if (token.type == TokenType::Identifier) {
        // Could be a property name or a literal like 'red'
        if (isalpha(token.value[0])) {
             // For now, treat identifiers that are not numbers as property accesses
            if (token.value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_") == std::string::npos) {
                 return std::make_shared<PropertyAccessNode>(token.value);
            }
        }
       return std::make_shared<LiteralExprNode>(token.value);
    } else if (token.type == TokenType::StringLiteral) {
        return std::make_shared<LiteralExprNode>(token.value);
    }
    // Handle other primary expressions like parentheses if needed
    throw std::runtime_error("Unexpected token in expression: " + token.value);
}

ExprNodePtr ExpressionParser::parse_conditional(ExprNodePtr condition) {
    consume_token(); // consume '?'
    auto true_expr = parse();
    if (current_token().type != TokenType::Colon) {
        throw std::runtime_error("Expected ':' in conditional expression.");
    }
    consume_token(); // consume ':'
    auto false_expr = parse();
    return std::make_shared<ConditionalExprNode>(condition, true_expr, false_expr);
}

ExprNodePtr ExpressionParser::parse_binary_op_rhs(int expr_prec, ExprNodePtr lhs) {
    while (true) {
        int tok_prec = get_token_precedence();
        if (tok_prec < expr_prec) {
            return lhs;
        }

        Token op_token = current_token();
        consume_token();

        ExprNodePtr rhs = parse_primary();
        int next_prec = get_token_precedence();

        if (tok_prec < next_prec) {
            rhs = parse_binary_op_rhs(tok_prec + 1, rhs);
        }

        lhs = std::make_shared<BinaryOpExprNode>(lhs, op_token.value, rhs);

        // After parsing a binary op, check for a conditional
        if (current_token().type == TokenType::Question) {
            return parse_conditional(lhs);
        }
    }
}

ExprNodePtr ExpressionParser::parse() {
    ExprNodePtr lhs = parse_primary();
    if (current_token().type == TokenType::Question) {
        return parse_conditional(lhs);
    }
    return parse_binary_op_rhs(0, lhs);
}