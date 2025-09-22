#include "Parser.h"
#include "ValueLexer.h"
#include "ExpressionParser.h"
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
        if (tokens[current + 1].value == "Template" || tokens[current + 1].value == "Custom") {
            return parse_template_definition();
        } else if (tokens[current + 1].value == "Namespace") {
            return parse_namespace_definition();
        } else if (tokens[current + 1].value == "Configuration") {
            return parse_configuration_definition();
        } else if (tokens[current + 1].value == "Import") {
            return parse_import_statement();
        }
    }

    if (peek().type == TokenType::Identifier) {
        if (peek().value == "use") {
            return parse_use_statement();
        }
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
            if (peek().type == TokenType::At || peek().type == TokenType::Inherit) {
                node->children.push_back(parse_node());
            } else {
                auto attr_node = parse_attribute(true);
                consume(TokenType::Semicolon, "Expect ';' after attribute in template.");
                node->children.push_back(std::move(attr_node));
            }
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
            node->variables.push_back(parse_attribute(true)); // Style context is true for vars too
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
        node->settings.push_back(parse_attribute(false));
        consume(TokenType::Semicolon, "Expect ';' after setting.");
    }

    consume(TokenType::RightBrace, "Expect '}' to close configuration body.");
    return node;
}

std::unique_ptr<BaseNode> Parser::parse_use_statement() {
    consume(TokenType::Identifier, "Expect 'use' keyword."); // consume 'use'
    Token value = consume(TokenType::Identifier, "Expect value after 'use'.");
    consume(TokenType::Semicolon, "Expect ';' after use statement.");
    return std::make_unique<UseNode>(value.value);
}

std::unique_ptr<BaseNode> Parser::parse_import_statement() {
    consume(TokenType::LeftBracket, "Expect '[' for import statement.");
    consume(TokenType::Identifier, "Expect 'Import' keyword.");
    consume(TokenType::RightBracket, "Expect ']' for import statement.");

    consume(TokenType::At, "Expect '@' for import type.");
    std::string import_type = consume(TokenType::Identifier, "Expect import type.").value;

    consume(TokenType::Identifier, "Expect 'from' keyword."); // Should check value

    // Path can be quoted or unquoted
    Token path_token = advance();
    std::string path = path_token.value;

    consume(TokenType::Semicolon, "Expect ';' after import statement.");

    return std::make_unique<ImportNode>(import_type, path);
}

std::unique_ptr<AttributeNode> Parser::parse_attribute(bool is_style_context) {
    std::string key = consume(TokenType::Identifier, "Expect attribute key.").value;

    if (is_style_context && check(TokenType::Semicolon)) {
        return std::make_unique<AttributeNode>(key, std::optional<std::string>(std::nullopt));
    }

    if (!match(TokenType::Colon)) {
        consume(TokenType::Equals, "Expect ':' or '=' after attribute key.");
    }

    if (is_style_context) {
        std::string value_string;
        while(peek().type != TokenType::Semicolon && !is_at_end()) {
            value_string += advance().value;
            if (peek().type != TokenType::Semicolon) value_string += " ";
        }

        ValueLexer v_lexer(value_string);
        auto v_tokens = v_lexer.tokenize();

        bool is_expression = false;
        for(const auto& t : v_tokens) {
            if (t.type == ValueTokenType::Operator) {
                is_expression = true;
                break;
            }
        }

        if (is_expression) {
            ExpressionParser e_parser(v_tokens);
            return std::make_unique<AttributeNode>(key, e_parser.parse());
        } else {
            if (!value_string.empty() && value_string.back() == ' ') value_string.pop_back();
            return std::make_unique<AttributeNode>(key, std::optional<std::string>(value_string));
        }

    } else { // Not a style context
        if (peek().type == TokenType::Identifier && tokens[current + 1].type == TokenType::LeftParen) {
            return std::make_unique<AttributeNode>(key, parse_variable_usage());
        }

        Token value_token = advance();
        return std::make_unique<AttributeNode>(key, std::optional<std::string>(value_token.value));
    }
}

std::unique_ptr<VarUsageNode> Parser::parse_variable_usage() {
    Token group_name = consume(TokenType::Identifier, "Expect variable group name.");
    consume(TokenType::LeftParen, "Expect '(' after variable group name.");
    Token var_name = consume(TokenType::Identifier, "Expect variable name inside parentheses.");
    consume(TokenType::RightParen, "Expect ')' to close variable usage.");
    return std::make_unique<VarUsageNode>(group_name.value, var_name.value);
}

std::unique_ptr<BaseNode> Parser::parse_node() {
    if (peek().type == TokenType::Comment) {
        return std::make_unique<CommentNode>(advance().value);
    }

    if (peek().type == TokenType::At || peek().type == TokenType::Inherit) {
        bool is_inherit = match(TokenType::Inherit);
        consume(TokenType::At, "Expect '@' after 'inherit' keyword or for template usage.");
        Token type = consume(TokenType::Identifier, "Expect template type after '@'.");
        Token name = consume(TokenType::Identifier, "Expect template name after type.");
        std::optional<std::string> from_ns;
        if (match(TokenType::From)) {
            from_ns = consume(TokenType::Identifier, "Expect namespace name after 'from'.").value;
        }

        if (type.value == "Style") {
            auto node = std::make_unique<StyleUsageNode>(name.value);
            node->from_namespace = from_ns;
            if (match(TokenType::LeftBrace)) {
                while(!check(TokenType::RightBrace) && !is_at_end()) {
                    if (match(TokenType::Delete)) {
                        auto delete_node = std::make_unique<DeleteNode>();
                        do {
                            delete_node->keys_to_delete.push_back(consume(TokenType::Identifier, "Expect property name after 'delete'.").value);
                        } while (match(TokenType::Comma));
                        consume(TokenType::Semicolon, "Expect ';' after delete statement.");
                        node->specializations.push_back(std::move(delete_node));
                    } else {
                        node->specializations.push_back(parse_attribute(true));
                        consume(TokenType::Semicolon, "Expect ';' after specialization.");
                    }
                }
                consume(TokenType::RightBrace, "Expect '}' to close specialization block.");
            } else {
                consume(TokenType::Semicolon, "Expect ';' after template usage.");
            }
            return node;
        } else if (type.value == "Element") {
            consume(TokenType::Semicolon, "Expect ';' after template usage.");
            auto node = std::make_unique<ElementUsageNode>(name.value);
            node->from_namespace = from_ns;
            return node;
        }
        return nullptr;
    }

    if (peek().type == TokenType::Identifier) {
        if (peek().value == "text" && (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals)) {
            advance();
            advance();
            Token value_token = consume(TokenType::StringLiteral, "Expect string literal for text attribute.");
            consume(TokenType::Semicolon, "Expect ';' after text attribute.");
            return std::make_unique<TextNode>(value_token.value);
        }

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
        if (peek().type == TokenType::Identifier && (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals)) {
            if (peek().value == "text") {
                 element->add_child(parse_node());
            } else {
                element->add_attribute(parse_attribute(false));
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
