#pragma once

#include "CHTL/Lexer/Token.h"
#include "CHTL/Node/Node.h"
#include "CHTL/Node/ElementNode.h"
#include "CHTL/Node/TextNode.h"
#include "CHTL/Node/CommentNode.h"
#include "ParseResult.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    ParseResult parse();

private:
    NodePtr parse_node(CssRuleList& global_rules);
    NodePtr parse_element(CssRuleList& global_rules);
    NodePtr parse_text_block();

    // Helper methods for parsing parts of an element
    void parse_style_block(StylePropertyList& styles, CssRuleList& global_rules, ElementNode& parent_element);
    Attribute parse_attribute();
    StyleProperty parse_style_property();
    std::string parse_selector();

    // Expression parsing methods
    ExprPtr parse_expression();
    ExprPtr parse_conditional();
    ExprPtr parse_logical_or();
    ExprPtr parse_logical_and();
    ExprPtr parse_equality();
    ExprPtr parse_comparison();
    ExprPtr parse_term();
    ExprPtr parse_factor();
    ExprPtr parse_unary();
    ExprPtr parse_primary();
    ExprPtr parse_reference();

    // Token stream management
    bool is_at_end() const;
    Token advance();
    Token peek() const;
    Token previous() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& error_message);

    std::vector<Token> tokens;
    size_t current = 0;
};
