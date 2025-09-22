#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    std::unique_ptr<BaseNode> parse_top_level_statement();
    std::unique_ptr<BaseNode> parse_template_definition();
    std::unique_ptr<BaseNode> parse_namespace_definition();
    std::unique_ptr<BaseNode> parse_configuration_definition();
    std::unique_ptr<BaseNode> parse_import_statement();
    std::unique_ptr<BaseNode> parse_use_statement();
    std::unique_ptr<VarUsageNode> parse_variable_usage();
    std::vector<Token> tokens;
    size_t current = 0;

    std::unique_ptr<ElementNode> parse_element();
    std::unique_ptr<BaseNode> parse_node();
    void parse_element_content(ElementNode* element);
    std::unique_ptr<AttributeNode> parse_attribute(bool is_style_context = false);
    std::unique_ptr<TextNode> parse_text_block();
    std::unique_ptr<StyleNode> parse_style_block();


    Token& peek();
    Token& previous();
    Token& advance();
    bool is_at_end();
    bool check(TokenType type);
    bool match(TokenType type);
    Token& consume(TokenType type, const std::string& message);
};

#endif //CHTL_PARSER_H
