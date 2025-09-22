#include "Parser.h"
#include <map>

// Helper map for template types
static const std::map<std::string, TemplateType> template_type_map = {
    {"@Style", TemplateType::STYLE},
    {"@Element", TemplateType::ELEMENT},
    {"@Var", TemplateType::VAR}
};

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<BaseNode>> Parser::parse() {
    std::vector<std::shared_ptr<BaseNode>> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

// --- Grammar Rules ---

std::shared_ptr<BaseNode> Parser::declaration() {
    try {
        if (check(TokenType::TOKEN_LBRACK)) {
            if (peek_next().type == TokenType::TOKEN_KEYWORD_TEMPLATE) {
                return template_declaration();
            }
            if (peek_next().type == TokenType::TOKEN_KEYWORD_CONFIGURATION) {
                return configuration_declaration();
            }
        }
        if (check(TokenType::TOKEN_IDENTIFIER)) { return element_declaration(); }
        if (check(TokenType::TOKEN_KEYWORD_TEXT)) { return text_declaration(); }
        if (check(TokenType::TOKEN_KEYWORD_STYLE)) { return style_block_declaration(); }
        if (check(TokenType::TOKEN_AT_IDENTIFIER)) { return template_usage(); }
        throw ParseError("Expected a declaration.");
    } catch (const ParseError& error) {
        throw;
    }
}

std::shared_ptr<ConfigurationNode> Parser::configuration_declaration() {
    consume(TokenType::TOKEN_LBRACK, "Expect '[' before Configuration keyword.");
    consume(TokenType::TOKEN_KEYWORD_CONFIGURATION, "Expect 'Configuration' keyword.");
    consume(TokenType::TOKEN_RBRACK, "Expect ']' after Configuration keyword.");

    auto config_node = std::make_shared<ConfigurationNode>();
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after [Configuration].");

    while (!check(TokenType::TOKEN_RBRACE) && !is_at_end()) {
        config_node->children.push_back(config_property());
    }

    consume(TokenType::TOKEN_RBRACE, "Expect '}' after configuration block.");
    return config_node;
}

std::shared_ptr<ConfigPropertyNode> Parser::config_property() {
    Token key = consume(TokenType::TOKEN_IDENTIFIER, "Expect configuration key.");
    consume(TokenType::TOKEN_EQUAL, "Expect '=' after configuration key.");

    Token value;
    if (match(TokenType::TOKEN_IDENTIFIER, TokenType::TOKEN_NUMBER, TokenType::TOKEN_STRING_DOUBLE, TokenType::TOKEN_STRING_SINGLE)) {
        value = previous();
    } else {
        throw ParseError("Expect configuration value.");
    }

    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after configuration value.");
    return std::make_shared<ConfigPropertyNode>(key.lexeme, value.lexeme);
}


std::shared_ptr<TemplateDefinitionNode> Parser::template_declaration() {
    consume(TokenType::TOKEN_LBRACK, "Expect '[' before template keyword.");
    consume(TokenType::TOKEN_KEYWORD_TEMPLATE, "Expect 'Template' keyword.");
    consume(TokenType::TOKEN_RBRACK, "Expect ']' after template keyword.");
    Token type_token = consume(TokenType::TOKEN_AT_IDENTIFIER, "Expect template type (e.g., @Style).");
    auto it = template_type_map.find(type_token.lexeme);
    if (it == template_type_map.end()) { throw ParseError("Unknown template type: " + type_token.lexeme); }
    TemplateType type = it->second;
    Token name = consume(TokenType::TOKEN_IDENTIFIER, "Expect template name.");
    auto template_def = std::make_shared<TemplateDefinitionNode>(type, name.lexeme);
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after template name.");
    while (!check(TokenType::TOKEN_RBRACE) && !is_at_end()) {
        if (type == TemplateType::STYLE) { template_def->children.push_back(style_property()); }
        else { template_def->children.push_back(declaration()); }
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after template body.");
    return template_def;
}

std::shared_ptr<TemplateUsageNode> Parser::template_usage() {
    Token type_token = consume(TokenType::TOKEN_AT_IDENTIFIER, "Expect template type (e.g., @Style).");
    auto it = template_type_map.find(type_token.lexeme);
    if (it == template_type_map.end()) { throw ParseError("Unknown template type: " + type_token.lexeme); }
    TemplateType type = it->second;
    Token name = consume(TokenType::TOKEN_IDENTIFIER, "Expect template name.");
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after template usage.");
    return std::make_shared<TemplateUsageNode>(type, name.lexeme);
}

std::shared_ptr<ElementNode> Parser::element_declaration() {
    Token name = consume(TokenType::TOKEN_IDENTIFIER, "Expect element name.");
    auto element = std::make_shared<ElementNode>(name.lexeme);
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after element name.");
    while (!check(TokenType::TOKEN_RBRACE) && !is_at_end()) {
        if (check(TokenType::TOKEN_IDENTIFIER) && peek_next().type == TokenType::TOKEN_COLON) { attribute(*element); }
        else { element->children.push_back(declaration()); }
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after element block.");
    return element;
}

void Parser::attribute(ElementNode& element) {
    Token name = consume(TokenType::TOKEN_IDENTIFIER, "Expect attribute name.");
    consume(TokenType::TOKEN_COLON, "Expect ':' after attribute name.");
    Token value;
    if (match(TokenType::TOKEN_STRING_DOUBLE, TokenType::TOKEN_STRING_SINGLE, TokenType::TOKEN_IDENTIFIER, TokenType::TOKEN_NUMBER)) { value = previous(); }
    else { throw ParseError("Expect attribute value (string, identifier, or number)."); }
    element.attributes[name.lexeme] = value.lexeme;
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after attribute value.");
}

std::shared_ptr<TextNode> Parser::text_declaration() {
    consume(TokenType::TOKEN_KEYWORD_TEXT, "Expect 'text' keyword.");
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'text' keyword.");
    Token content = consume(TokenType::TOKEN_STRING_DOUBLE, "Expect string literal inside text block.");
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after text block.");
    return std::make_shared<TextNode>(content.lexeme);
}

std::shared_ptr<StyleBlockNode> Parser::style_block_declaration() {
    consume(TokenType::TOKEN_KEYWORD_STYLE, "Expect 'style' keyword.");
    auto style_block = std::make_shared<StyleBlockNode>();
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'style' keyword.");
    while (!check(TokenType::TOKEN_RBRACE) && !is_at_end()) {
        if (check(TokenType::TOKEN_AT_IDENTIFIER)) { style_block->children.push_back(template_usage()); }
        else { style_block->children.push_back(style_property()); }
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after style block.");
    return style_block;
}

std::shared_ptr<StylePropertyNode> Parser::style_property() {
    Token name = consume(TokenType::TOKEN_IDENTIFIER, "Expect style property name.");
    consume(TokenType::TOKEN_COLON, "Expect ':' after style property name.");
    std::string full_value;
    while(!check(TokenType::TOKEN_SEMICOLON) && !is_at_end()) { full_value += advance().lexeme; }
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after style property value.");
    return std::make_shared<StylePropertyNode>(name.lexeme, full_value);
}

Token Parser::peek() { return tokens[current]; }
Token Parser::peek_next() {
    if (current + 1 >= tokens.size()) return tokens.back();
    return tokens[current + 1];
}
Token Parser::previous() { return tokens[current - 1]; }
bool Parser::is_at_end() { return peek().type == TokenType::TOKEN_EOF; }
Token Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}
bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}
template<typename... Types>
bool Parser::match(Types... types) {
    for (TokenType type : {types...}) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    const Token& t = peek();
    throw ParseError(message + " at line " + std::to_string(t.line) + ", col " + std::to_string(t.col));
}
