#include "Parser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "Util/FileLoader.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <sstream>

Parser::Parser(const std::vector<Token>& tokens, ParsingContext& context) : tokens(tokens), context(context) {}

// Forward declaration of a helper for the helper
bool is_simple_arithmetic(const std::vector<Token>& tokens);

std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> statements;
    while (!is_at_end()) {
        auto node = declaration();
        if (node) {
            if (node->getType() == NodeType::TEMPLATE_DEFINITION) {
                auto template_def = std::unique_ptr<TemplateDefinitionNode>(static_cast<TemplateDefinitionNode*>(node.release()));
                context.style_templates[template_def->name] = std::move(template_def);
            } else {
                statements.push_back(std::move(node));
            }
        }
    }
    return statements;
}

std::unique_ptr<BaseNode> Parser::declaration() {
    try {
        while(peek().type == TokenType::LBRACKET && peek_next().type == TokenType::IMPORT) {
            import_declaration();
        }
        if (peek().type == TokenType::LBRACKET && (peek_next().type == TokenType::TEMPLATE || peek_next().type == TokenType::CUSTOM)) {
            return template_declaration();
        }
        if (match({TokenType::HASH})) {
            auto node = std::make_unique<CommentNode>();
            node->content = previous().value;
            return node;
        }
        if (match({TokenType::IDENTIFIER})) {
            return element_declaration();
        }
    } catch (const std::runtime_error& error) {
        std::cerr << "Parse Error: " << error.what() << std::endl;
        synchronize();
        return nullptr;
    }

    if (!is_at_end()) {
        std::cerr << "Parse Error: Unexpected token '" << peek().value << "' on line " << peek().line << std::endl;
        advance();
    }
    return nullptr;
}

void Parser::import_declaration() {
    consume(TokenType::LBRACKET, "Expect '[' to start an import statement.");
    consume(TokenType::IMPORT, "Expect 'Import' keyword.");
    consume(TokenType::RBRACKET, "Expect ']' after 'Import' keyword.");
    consume(TokenType::AT, "Expect '@' for import type.");
    consume(TokenType::CHTL, "Only '@Chtl' imports are currently supported.");
    consume(TokenType::FROM, "Expect 'from' keyword.");
    Token path = consume(TokenType::STRING, "Expect a string literal for the file path.");
    consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
    std::string file_content = CHTL::load_file(path.value);
    Lexer imported_lexer(file_content);
    std::vector<Token> imported_tokens = imported_lexer.tokenize();
    Parser imported_parser(imported_tokens, context);
    imported_parser.parse();
}

std::unique_ptr<BaseNode> Parser::template_declaration() {
    auto template_node = std::make_unique<TemplateDefinitionNode>();
    consume(TokenType::LBRACKET, "Expect '[' to start a definition.");
    if (match({TokenType::TEMPLATE})) {
        template_node->modifier = TemplateModifier::IS_TEMPLATE;
    } else if (match({TokenType::CUSTOM})) {
        template_node->modifier = TemplateModifier::IS_CUSTOM;
    } else {
        throw std::runtime_error("Expect 'Template' or 'Custom' keyword.");
    }
    consume(TokenType::RBRACKET, "Expect ']' after keyword.");
    consume(TokenType::AT, "Expect '@' for template type.");
    Token template_type = consume(TokenType::IDENTIFIER, "Expect template type (e.g. 'Style').");
    if (template_type.value != "Style") {
        throw std::runtime_error("Only '@Style' templates are currently supported.");
    }
    Token name = consume(TokenType::IDENTIFIER, "Expect a name for the style template.");
    template_node->name = name.value;
    consume(TokenType::LBRACE, "Expect '{' to start template body.");
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        Token key = consume(TokenType::IDENTIFIER, "Expect style property name.");
        if (match({TokenType::COLON})) {
            std::string value_str = parse_value_sequence();
            template_node->style_properties.push_back({key.value, value_str});
        } else {
            consume(TokenType::SEMICOLON, "Expect ';' after valueless property.");
            template_node->style_properties.push_back({key.value, std::nullopt});
        }
    }
    consume(TokenType::RBRACE, "Expect '}' to end template body.");
    return template_node;
}

std::unique_ptr<ElementNode> Parser::element_declaration() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = previous().value;
    consume(TokenType::LBRACE, "Expect '{' after element name.");
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        if (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::COLON) {
            parse_attributes(*element);
        } else if (peek().type == TokenType::TEXT) {
             element->children.push_back(text_block_inside_element());
        } else if (peek().type == TokenType::STYLE) {
            element->children.push_back(parse_style_block());
        }
        else {
            element->children.push_back(declaration());
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after element block.");
    return element;
}

void Parser::parse_attributes(ElementNode& element) {
    do {
        Token key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
        consume(TokenType::COLON, "Expect ':' after attribute name.");
        std::string value_str = parse_value_sequence();
        element.attributes.push_back({key.value, value_str});
    } while (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::COLON);
}

// New robust implementation
std::string Parser::parse_value_sequence() {
    // Handle quoted strings separately as they don't mix with unquoted literals
    if (peek().type == TokenType::STRING) {
        std::string value = advance().value;
        consume(TokenType::SEMICOLON, "Expect ';' after string value.");
        return value;
    }

    // Gather all tokens until the semicolon
    std::vector<Token> value_tokens;
    while (peek().type != TokenType::SEMICOLON && !is_at_end()) {
        value_tokens.push_back(advance());
    }

    std::string result_string;

    // Process the collected tokens
    if (is_simple_arithmetic(value_tokens)) {
        // Evaluation logic
        Term lhs = {std::stod(value_tokens[0].value), ""};
        if (value_tokens.size() > 1 && value_tokens[1].type == TokenType::IDENTIFIER) {
            lhs.unit = value_tokens[1].value;
        }

        for (size_t i = lhs.unit.empty() ? 1 : 2; i < value_tokens.size();) {
            Token op = value_tokens[i++];

            if (i >= value_tokens.size() || value_tokens[i].type != TokenType::NUMBER) {
                throw std::runtime_error("Expected number after operator in expression.");
            }
            Term rhs = {std::stod(value_tokens[i++].value), ""};
            if (i < value_tokens.size() && value_tokens[i].type == TokenType::IDENTIFIER) {
                rhs.unit = value_tokens[i++].value;
            }

            if (lhs.unit != rhs.unit) {
                throw std::runtime_error("Mismatched units in arithmetic expression ('" + lhs.unit + "' and '" + rhs.unit + "').");
            }

            if (op.type == TokenType::PLUS) lhs.value += rhs.value;
            else if (op.type == TokenType::MINUS) lhs.value -= rhs.value;
        }
        std::stringstream ss;
        ss << lhs.value << lhs.unit;
        result_string = ss.str();

    } else {
        // Join with spaces for generic literals
        for (size_t i = 0; i < value_tokens.size(); ++i) {
            result_string += value_tokens[i].value;
            if (i < value_tokens.size() - 1) {
                result_string += " ";
            }
        }
    }

    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return result_string;
}

bool is_simple_arithmetic(const std::vector<Token>& tokens) {
    if (tokens.empty() || tokens[0].type != TokenType::NUMBER) {
        return false;
    }
    // A simple check: if any token is a PLUS or MINUS, we'll try to evaluate it.
    for (const auto& token : tokens) {
        if (token.type == TokenType::PLUS || token.type == TokenType::MINUS) {
            return true;
        }
    }
    return false;
}


Term Parser::parse_term() {
    Token number = consume(TokenType::NUMBER, "Expect a number for a term in an expression.");
    std::string unit;
    if (peek().type == TokenType::IDENTIFIER) {
        unit = advance().value;
    }
    return {std::stod(number.value), unit};
}

std::unique_ptr<TextNode> Parser::text_block_inside_element() {
    consume(TokenType::TEXT, "Expect 'text' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'text' keyword.");
    Token content = consume(TokenType::STRING, "Expect string literal inside text block.");
    consume(TokenType::RBRACE, "Expect '}' after text content.");
    auto textNode = std::make_unique<TextNode>();
    textNode->content = content.value;
    return textNode;
}

std::unique_ptr<StyleNode> Parser::parse_style_block() {
    consume(TokenType::STYLE, "Expect 'style' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'style' keyword.");
    auto styleNode = std::make_unique<StyleNode>();
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        if (match({TokenType::AT})) {
            Token template_type = consume(TokenType::IDENTIFIER, "Expect template type (e.g. 'Style').");
            if (template_type.value != "Style") {
                throw std::runtime_error("Only '@Style' templates are currently supported.");
            }
            Token template_name = consume(TokenType::IDENTIFIER, "Expect template name.");
            consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
            auto it = context.style_templates.find(template_name.value);
            if (it != context.style_templates.end()) {
                const auto& props = it->second->style_properties;
                styleNode->properties.insert(styleNode->properties.end(), props.begin(), props.end());
            } else {
                throw std::runtime_error("Undefined style template: '" + template_name.value + "'");
            }
        } else {
            Token key = consume(TokenType::IDENTIFIER, "Expect style property name.");
            consume(TokenType::COLON, "Expect ':' after style property name.");
            std::string value_str = parse_value_sequence();
            styleNode->properties.push_back({key.value, value_str});
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after style block.");
    return styleNode;
}

// --- Helper Methods ---
Token Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}
Token Parser::peek() {
    return tokens[current];
}
Token Parser::peek_next() {
    if (is_at_end() || current + 1 >= tokens.size()) {
        return tokens.back();
    }
    return tokens[current + 1];
}
Token Parser::previous() {
    return tokens[current - 1];
}
bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}
bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}
bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Line " + std::to_string(peek().line) + ": " + message);
}
void Parser::synchronize() {
    advance();
    while (!is_at_end()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::TEXT:
            case TokenType::STYLE:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}
