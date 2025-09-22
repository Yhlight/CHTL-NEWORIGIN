#include "Parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!is_at_end() && peek().type != TokenType::EndOfFile) {
        auto node = parse_top_level_statement();
        if (node) {
            program->add_child(std::move(node));
        }
    }
    return program;
}

std::unique_ptr<BaseNode> Parser::parse_top_level_statement() {
    if (peek().type == TokenType::Comment) {
        return std::make_unique<CommentNode>(advance().value);
    }

    if (peek().type == TokenType::LeftBracket) {
        // Could be [Template] or [Namespace]
        if (tokens[current + 1].value == "Template" || tokens[current + 1].value == "Custom") {
            return parse_template_definition();
        } else if (tokens[current + 1].value == "Namespace") {
            return parse_namespace_definition();
        } else if (tokens[current + 1].value == "Configuration") {
            return parse_configuration_definition();
        }
    }

    if (peek().type == TokenType::Identifier) {
        return parse_element();
    }

    std::cerr << "Parse Error: Unexpected token '" << peek().value << "' at top level on line " << peek().line << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<BaseNode> Parser::parse_template_definition() {
    consume(TokenType::LeftBracket, "Expect '[' for template definition.");
    Token keyword = consume(TokenType::Identifier, "Expect 'Template' or 'Custom' keyword.");
    bool is_custom = (keyword.value == "Custom");
    consume(TokenType::RightBracket, "Expect ']' for template definition.");

    consume(TokenType::At, "Expect '@' for template type.");
    Token type = consume(TokenType::Identifier, "Expect template type (Style, Element, Var).");
    Token name = consume(TokenType::Identifier, "Expect template name.");

    consume(TokenType::LeftBrace, "Expect '{' to open template body.");

    if (type.value == "Style") {
        auto node = std::make_unique<TemplateStyleNode>(name.value, is_custom);
        while(!check(TokenType::RightBrace) && !is_at_end()) {
            node->styles.push_back(parse_attribute(is_custom));
            consume(TokenType::Semicolon, "Expect ';' after attribute in template.");
        }
        consume(TokenType::RightBrace, "Expect '}' to close template body.");
        return node;
    }
    if (type.value == "Element") {
        auto node = std::make_unique<TemplateElementNode>(name.value, is_custom);
        while(!check(TokenType::RightBrace) && !is_at_end()) {
            node->children.push_back(parse_node());
        }
        consume(TokenType::RightBrace, "Expect '}' to close template body.");
        return node;
    }
    if (type.value == "Var") {
        auto node = std::make_unique<TemplateVarNode>(name.value, is_custom);
        while(!check(TokenType::RightBrace) && !is_at_end()) {
            node->variables.push_back(parse_attribute(is_custom));
            consume(TokenType::Semicolon, "Expect ';' after variable in template.");
        }
        consume(TokenType::RightBrace, "Expect '}' to close template body.");
        return node;
    }

    // Unknown template type
    while(!check(TokenType::RightBrace) && !is_at_end()) { advance(); }
    consume(TokenType::RightBrace, "Expect '}' to close template body.");
    return nullptr;
}

std::unique_ptr<BaseNode> Parser::parse_namespace_definition() {
    consume(TokenType::LeftBracket, "Expect '[' for namespace definition.");
    consume(TokenType::Identifier, "Expect 'Namespace' keyword.");
    consume(TokenType::RightBracket, "Expect ']' for namespace definition.");

    Token name = consume(TokenType::Identifier, "Expect namespace name.");
    auto node = std::make_unique<NamespaceNode>(name.value);

    consume(TokenType::LeftBrace, "Expect '{' to open namespace body.");

    while(!check(TokenType::RightBrace) && !is_at_end()) {
        node->add_child(parse_top_level_statement());
    }

    consume(TokenType::RightBrace, "Expect '}' to close namespace body.");
    return node;
}

std::unique_ptr<BaseNode> Parser::parse_configuration_definition() {
    consume(TokenType::LeftBracket, "Expect '[' for configuration definition.");
    consume(TokenType::Identifier, "Expect 'Configuration' keyword.");
    consume(TokenType::RightBracket, "Expect ']' for configuration definition.");

    auto node = std::make_unique<ConfigurationNode>();
    consume(TokenType::LeftBrace, "Expect '{' to open configuration body.");

    while(!check(TokenType::RightBrace) && !is_at_end()) {
        // Not supporting [Name] block yet
        node->settings.push_back(parse_attribute(false));
        consume(TokenType::Semicolon, "Expect ';' after setting.");
    }

    consume(TokenType::RightBrace, "Expect '}' to close configuration body.");
    return node;
}

std::unique_ptr<AttributeNode> Parser::parse_attribute(bool is_custom_context) {
    std::string key = consume(TokenType::Identifier, "Expect attribute key.").value;

    // Handle valueless attributes in custom context, e.g. "color;"
    if (is_custom_context && check(TokenType::Semicolon)) {
        return std::make_unique<AttributeNode>(key, std::optional<std::string>(std::nullopt));
    }

    if (!match(TokenType::Colon)) {
        consume(TokenType::Equals, "Expect ':' or '=' after attribute key.");
    }
    Token value_token = advance(); // StringLiteral or Identifier

    // For now, all values are treated as simple strings.
    // In the future, we would parse expressions here for style properties.
    return std::make_unique<AttributeNode>(key, std::optional<std::string>(value_token.value));
}

std::unique_ptr<BaseNode> Parser::parse_node() {
    if (peek().type == TokenType::Comment) {
        return std::make_unique<CommentNode>(advance().value);
    }

    if (peek().type == TokenType::At) {
        advance(); // consume '@'
        Token type = consume(TokenType::Identifier, "Expect template type after '@'.");
        Token name = consume(TokenType::Identifier, "Expect template name after type.");
        consume(TokenType::Semicolon, "Expect ';' after template usage.");

        if (type.value == "Style") {
            return std::make_unique<StyleUsageNode>(name.value);
        } else if (type.value == "Element") {
            return std::make_unique<ElementUsageNode>(name.value);
        }
        return nullptr; // Unknown template usage
    }

    if (peek().type == TokenType::Identifier) {
        // text: "..." is an attribute-like statement, not a nested element
        if (peek().value == "text" && (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals)) {
            advance(); // consume 'text'
            advance(); // consume ':' or '='
            Token value_token = consume(TokenType::StringLiteral, "Expect string literal for text attribute.");
            consume(TokenType::Semicolon, "Expect ';' after text attribute.");
            return std::make_unique<TextNode>(value_token.value);
        }

        // It's a nested element, a text block, or a style block
        if (peek().value == "text") {
            advance();
            consume(TokenType::LeftBrace, "Expect '{' after 'text' keyword.");
            return parse_text_block();
        } else if (peek().value == "style") {
            advance();
            consume(TokenType::LeftBrace, "Expect '{' after 'style' keyword.");
            return parse_style_block();
        }
        return parse_element();
    }

    std::cerr << "Parse Error: Unexpected token '" << peek().value << "' inside element on line " << peek().line << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parse_element() {
    Token tag_token = consume(TokenType::Identifier, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tag_token.value);

    consume(TokenType::LeftBrace, "Expect '{' after element tag name.");

    while (!check(TokenType::RightBrace) && !is_at_end()) {
        // Check for attributes first
        if (peek().type == TokenType::Identifier && (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals)) {
            if (peek().value == "text") {
                 element->add_child(parse_node());
            } else {
                element->add_attribute(parse_attribute(false)); // Not custom context
                consume(TokenType::Semicolon, "Expect ';' after attribute value.");
            }
        } else {
            element->add_child(parse_node());
        }
    }

    consume(TokenType::RightBrace, "Expect '}' to close element.");
    return element;
}

void Parser::parse_element_content(ElementNode* element) {
    // This function is now replaced by the logic inside parse_element's while loop.
}

std::unique_ptr<TextNode> Parser::parse_text_block() {
    Token content = consume(TokenType::StringLiteral, "Expect string literal inside text block.");
    consume(TokenType::RightBrace, "Expect '}' to close text block.");
    return std::make_unique<TextNode>(content.value);
}

std::unique_ptr<StyleNode> Parser::parse_style_block() {
    std::string content;
    int brace_level = 1;
    while(brace_level > 0 && !is_at_end()) {
        if (peek().type == TokenType::LeftBrace) brace_level++;
        if (peek().type == TokenType::RightBrace) brace_level--;

        if (brace_level > 0) {
            content += advance().value;
            if (peek().type != TokenType::RightBrace) content += " ";
        } else {
            consume(TokenType::RightBrace, "Expect '}' to close style block.");
        }
    }
    return std::make_unique<StyleNode>(content);
}

// Helper methods
Token& Parser::peek() { return tokens[current]; }
Token& Parser::previous() { return tokens[current - 1]; }
bool Parser::is_at_end() { return peek().type == TokenType::EndOfFile; }

Token& Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    std::cerr << "Parse Error: " << message << " at line " << peek().line << std::endl;
    exit(1);
}
