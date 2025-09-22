#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/Node.h"
#include "CHTL/CHTLContext.h"
#include <vector>
#include <memory>

struct Term {
    double value;
    std::string unit;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens, ParsingContext& context);
    std::vector<std::unique_ptr<BaseNode>> parse();

private:
    const std::vector<Token>& tokens;
    ParsingContext& context;
    size_t current = 0;

    // Token navigation helpers
    Token advance();
    Token peek();
    Token peek_next();
    Token previous();
    bool is_at_end();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    // Error handling and synchronization
    Token consume(TokenType type, const std::string& message);
    void synchronize();

    // Parsing methods for grammar rules
    std::unique_ptr<BaseNode> declaration();
    void import_declaration();
    std::unique_ptr<BaseNode> template_declaration();
    std::unique_ptr<ElementNode> element_declaration();
    void parse_attributes(ElementNode& element);
    std::string parse_value_sequence();
    Term parse_term();
    std::unique_ptr<TextNode> text_block_inside_element();
    std::unique_ptr<StyleNode> parse_style_block();
};
