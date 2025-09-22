#include "Parser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "Util/FileLoader.h"
#include <stdexcept>
#include <iostream>
#include <vector>

Parser::Parser(const std::vector<Token>& tokens, ParsingContext& context) : tokens(tokens), context(context) {}

std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> statements;
    while (!is_at_end()) {
        auto node = declaration();
        if (node) {
            // Template definitions are moved to the context and not added to the render tree
            if (node->getType() == NodeType::TEMPLATE_DEFINITION) {
                // Cast to the derived type to access name and move ownership
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
            Token value = consume(TokenType::STRING, "Expect string literal for style value.");
            template_node->style_properties.push_back({key.value, value.value});
        } else {
            template_node->style_properties.push_back({key.value, std::nullopt});
        }
        consume(TokenType::SEMICOLON, "Expect ';' after style property.");
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
        Token value = consume(TokenType::STRING, "Expect attribute value.");
        consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
        element.attributes.push_back({key.value, value.value});
    } while (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::COLON);
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
            Token value = consume(TokenType::STRING, "Expect string literal for style value.");
            consume(TokenType::SEMICOLON, "Expect ';' after style value.");
            styleNode->properties.push_back({key.value, value.value});
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
