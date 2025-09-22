#pragma once

#include "CHTL/Lexer/Token.h"
#include "CHTL/Node/Node.h"
#include "CHTL/Node/ElementNode.h"
#include "CHTL/Node/TextNode.h"
#include "CHTL/Node/CommentNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    NodeList parse();

private:
    NodePtr parse_node();
    NodePtr parse_element();
    NodePtr parse_text_block();

    // Helper methods for parsing parts of an element
    void parse_style_block(StylePropertyList& styles);
    Attribute parse_attribute();
    StyleProperty parse_style_property();

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
