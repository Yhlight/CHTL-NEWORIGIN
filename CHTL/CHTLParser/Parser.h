#pragma once

#include "Token.h"
#include "BaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "StyleBlockNode.h"
#include "StylePropertyNode.h"
#include "TemplateDefinitionNode.h"
#include "TemplateUsageNode.h"
#include "ConfigurationNode.h"
#include "ConfigPropertyNode.h"

#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<BaseNode>> parse();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    // --- Token Helpers ---
    Token peek();
    Token peek_next();
    Token previous();
    bool is_at_end();
    Token advance();
    bool check(TokenType type);
    template<typename... Types>
    bool match(Types... types);
    Token consume(TokenType type, const std::string& message);

    // --- Parsing Rules ---
    std::shared_ptr<BaseNode> declaration();
    std::shared_ptr<ElementNode> element_declaration();
    void attribute(ElementNode& element);
    std::shared_ptr<TextNode> text_declaration();
    std::shared_ptr<StyleBlockNode> style_block_declaration();
    std::shared_ptr<StylePropertyNode> style_property();
    std::shared_ptr<TemplateDefinitionNode> template_declaration();
    std::shared_ptr<TemplateUsageNode> template_usage();
    std::shared_ptr<ConfigurationNode> configuration_declaration();
    std::shared_ptr<ConfigPropertyNode> config_property();
};

// Custom exception for parsing errors
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};
