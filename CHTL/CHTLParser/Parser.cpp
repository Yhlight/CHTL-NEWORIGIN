#include "Parser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "Util/FileLoader.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

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
                context.template_definitions[template_def->name] = std::move(template_def);
            } else {
                statements.push_back(std::move(node));
            }
        }
    }
    return statements;
}

std::unique_ptr<BaseNode> Parser::declaration() {
    try {
        if (peek().type == TokenType::LBRACKET && peek_next().type == TokenType::IMPORT) {
            import_declaration();
            return nullptr;
        }
        if (peek().type == TokenType::LBRACKET && (peek_next().type == TokenType::TEMPLATE || peek_next().type == TokenType::CUSTOM)) {
            return template_declaration();
        }
        if (match({TokenType::HASH})) {
            auto node = std::make_unique<CommentNode>();
            std::string content;
            int comment_line = previous().line;
            while (!is_at_end() && peek().line == comment_line) {
                content += advance().value;
                 if (!is_at_end() && peek().line == comment_line) content += " ";
            }
            size_t first = content.find_first_not_of(" \t");
            if(std::string::npos != first) content = content.substr(first);
            node->content = content;
            return node;
        }
        if (match({TokenType::AT})) { // Template Usage
            auto usage_node = std::make_unique<TemplateUsageNode>();
            Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
            if(type.value == "Style") usage_node->template_type = TemplateType::STYLE;
            else if(type.value == "Element") usage_node->template_type = TemplateType::ELEMENT;
            else if(type.value == "Var") usage_node->template_type = TemplateType::VAR;
            else throw std::runtime_error("Unknown template type @" + type.value);

            usage_node->name = consume(TokenType::IDENTIFIER, "Expect template name.").value;
            consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
            return usage_node;
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
    consume(TokenType::LBRACKET, "Expect '['.");
    consume(TokenType::IMPORT, "Expect 'Import'.");
    consume(TokenType::RBRACKET, "Expect ']'.");
    consume(TokenType::AT, "Expect '@'.");
    consume(TokenType::CHTL, "Only '@Chtl' imports supported.");
    consume(TokenType::FROM, "Expect 'from'.");
    Token path = consume(TokenType::STRING, "Expect path.");
    consume(TokenType::SEMICOLON, "Expect ';'.");
    std::string content = CHTL::load_file(path.value);
    Lexer lexer(content);
    auto tokens = lexer.tokenize();
    Parser parser(tokens, context);
    parser.parse();
}

std::unique_ptr<BaseNode> Parser::template_declaration() {
    auto template_node = std::make_unique<TemplateDefinitionNode>();
    consume(TokenType::LBRACKET, "Expect '['.");
    if (match({TokenType::TEMPLATE})) template_node->modifier = TemplateModifier::IS_TEMPLATE;
    else if (match({TokenType::CUSTOM})) template_node->modifier = TemplateModifier::IS_CUSTOM;
    else throw std::runtime_error("Expect 'Template' or 'Custom'.");
    consume(TokenType::RBRACKET, "Expect ']'.");

    consume(TokenType::AT, "Expect '@'.");
    Token type = consume(TokenType::IDENTIFIER, "Expect 'Style', 'Element', or 'Var'.");

    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    template_node->name = name.value;

    consume(TokenType::LBRACE, "Expect '{'.");

    if (type.value == "Style") {
        template_node->template_type = TemplateType::STYLE;
        TemplateDefinitionNode::StyleBody body;
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            Token key = consume(TokenType::IDENTIFIER, "prop name");
            if (match({TokenType::COLON})) {
                body.push_back({key.value, parse_value_sequence()});
            } else {
                consume(TokenType::SEMICOLON, "';'");
                body.push_back({key.value, std::nullopt});
            }
        }
        template_node->body = std::move(body);
    } else if (type.value == "Element") {
        template_node->template_type = TemplateType::ELEMENT;
        TemplateDefinitionNode::ElementBody body;
        while (!check(TokenType::RBRACE) && !is_at_end()) {
            body.push_back(declaration());
        }
        template_node->body = std::move(body);
    } else if (type.value == "Var") {
        template_node->template_type = TemplateType::VAR;
        TemplateDefinitionNode::VarBody body;
        while(!check(TokenType::RBRACE) && !is_at_end()) {
            Token key = consume(TokenType::IDENTIFIER, "var name");
            consume(TokenType::COLON, "':'");
            body.push_back({key.value, parse_value_sequence()});
        }
        template_node->body = std::move(body);
    } else {
        throw std::runtime_error("Unsupported template type: @" + type.value);
    }

    consume(TokenType::RBRACE, "Expect '}'.");
    return template_node;
}

std::unique_ptr<ElementNode> Parser::element_declaration() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = previous().value;
    consume(TokenType::LBRACE, "Expect '{'.");
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
    consume(TokenType::RBRACE, "Expect '}'.");
    return element;
}

void Parser::parse_attributes(ElementNode& element) {
    do {
        Token key = consume(TokenType::IDENTIFIER, "attr name");
        consume(TokenType::COLON, "':'");
        element.attributes.push_back({key.value, parse_value_sequence()});
    } while (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::COLON);
}

std::string Parser::parse_value_sequence() {
    if (peek().type == TokenType::STRING) {
        std::string value = advance().value;
        consume(TokenType::SEMICOLON, "';'");
        return value;
    }

    // Check for variable substitution: IDENTIFIER(IDENTIFIER)
    if (peek().type == TokenType::IDENTIFIER && peek_next().type == TokenType::LPAREN) {
        Token var_group_name = advance();
        consume(TokenType::LPAREN, "'('");
        Token var_name = consume(TokenType::IDENTIFIER, "variable name");
        consume(TokenType::RPAREN, "')'");

        auto it = context.template_definitions.find(var_group_name.value);
        if (it == context.template_definitions.end()) throw std::runtime_error("Undefined variable group: " + var_group_name.value);
        if (it->second->template_type != TemplateType::VAR) throw std::runtime_error("'" + var_group_name.value + "' is not a variable group.");

        const auto& var_body = std::get<TemplateDefinitionNode::VarBody>(it->second->body);
        for(const auto& pair : var_body) {
            if (pair.first == var_name.value) {
                // For now, assume the result is the whole value
                consume(TokenType::SEMICOLON, "';'");
                return pair.second;
            }
        }
        throw std::runtime_error("Undefined variable '" + var_name.value + "' in group '" + var_group_name.value + "'.");
    }

    std::vector<Token> value_tokens;
    while (peek().type != TokenType::SEMICOLON && !is_at_end()) {
        value_tokens.push_back(advance());
    }
    std::string result_string;
    if (is_simple_arithmetic(value_tokens)) {
        Term lhs = {std::stod(value_tokens[0].value), ""};
        size_t idx = 1;
        if (value_tokens.size() > 1 && value_tokens[1].type == TokenType::IDENTIFIER) {
            lhs.unit = value_tokens[1].value;
            idx = 2;
        }
        for (size_t i = idx; i < value_tokens.size();) {
            Token op = value_tokens[i++];
            if (i >= value_tokens.size()|| value_tokens[i].type != TokenType::NUMBER) throw std::runtime_error("num expected");
            Term rhs = {std::stod(value_tokens[i++].value), ""};
            if (i < value_tokens.size() && value_tokens[i].type == TokenType::IDENTIFIER) rhs.unit = value_tokens[i++].value;
            if (lhs.unit != rhs.unit) throw std::runtime_error("unit mismatch");
            if (op.type == TokenType::PLUS) lhs.value += rhs.value;
            else if (op.type == TokenType::MINUS) lhs.value -= rhs.value;
        }
        std::stringstream ss;
        ss << lhs.value << lhs.unit;
        result_string = ss.str();
    } else {
        for (size_t i = 0; i < value_tokens.size(); ++i) {
            result_string += value_tokens[i].value;
            if (i < value_tokens.size() - 1) result_string += " ";
        }
    }
    consume(TokenType::SEMICOLON, "';'");
    return result_string;
}

bool is_simple_arithmetic(const std::vector<Token>& tokens) {
    if (tokens.empty() || tokens[0].type != TokenType::NUMBER) return false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::PLUS || token.type == TokenType::MINUS) return true;
    }
    return false;
}

Term Parser::parse_term() {
    Token num = consume(TokenType::NUMBER, "num expected");
    std::string unit;
    if (peek().type == TokenType::IDENTIFIER) unit = advance().value;
    return {std::stod(num.value), unit};
}

std::unique_ptr<TextNode> Parser::text_block_inside_element() {
    consume(TokenType::TEXT, "'text'");
    consume(TokenType::LBRACE, "'{'");
    Token content = consume(TokenType::STRING, "string");
    consume(TokenType::RBRACE, "'}'");
    auto textNode = std::make_unique<TextNode>();
    textNode->content = content.value;
    return textNode;
}

std::unique_ptr<StyleNode> Parser::parse_style_block() {
    consume(TokenType::STYLE, "'style'");
    consume(TokenType::LBRACE, "'{'");
    auto styleNode = std::make_unique<StyleNode>();
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        if (match({TokenType::AT})) {
            Token type = consume(TokenType::IDENTIFIER, "'Style'");
            if (type.value != "Style") throw std::runtime_error("Only @Style supported");
            Token name = consume(TokenType::IDENTIFIER, "name");
            consume(TokenType::SEMICOLON, "';'");
            auto it = context.template_definitions.find(name.value);
            if (it != context.template_definitions.end()) {
                const auto& body = std::get<TemplateDefinitionNode::StyleBody>(it->second->body);
                styleNode->inline_properties.insert(styleNode->inline_properties.end(), body.begin(), body.end());
            } else {
                throw std::runtime_error("Undefined template: " + name.value);
            }
        } else if (peek().type == TokenType::DOT || peek().type == TokenType::HASH) {
            Token type = advance();
            Token name = consume(TokenType::IDENTIFIER, "name");
            std::string selector = type.value + name.value;
            if (match({TokenType::LBRACE})) {
                StyleRule rule;
                rule.selector = selector;
                while(!check(TokenType::RBRACE) && !is_at_end()){
                    Token key = consume(TokenType::IDENTIFIER, "prop name");
                    consume(TokenType::COLON, "':'");
                    rule.properties.push_back({key.value, parse_value_sequence()});
                }
                consume(TokenType::RBRACE, "'}'");
                styleNode->style_rules.push_back(rule);
            } else {
                consume(TokenType::SEMICOLON, "';'");
                styleNode->style_rules.push_back({selector, {}});
            }
        }
        else {
            Token key = consume(TokenType::IDENTIFIER, "prop name");
            consume(TokenType::COLON, "':'");
            styleNode->inline_properties.push_back({key.value, parse_value_sequence()});
        }
    }
    consume(TokenType::RBRACE, "'}'");
    return styleNode;
}

// --- Helper Methods ---
Token Parser::advance() { if (!is_at_end()) current++; return previous(); }
Token Parser::peek() { if (current >= tokens.size()) return tokens.back(); return tokens[current]; }
Token Parser::peek_next() { if (current + 1 >= tokens.size()) return tokens.back(); return tokens[current + 1]; }
Token Parser::previous() { return tokens[current - 1]; }
bool Parser::is_at_end() { return current >= tokens.size() - 1; }
bool Parser::check(TokenType type) { if (is_at_end() && type != TokenType::END_OF_FILE) return false; return peek().type == type; }
bool Parser::match(const std::vector<TokenType>& types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }
Token Parser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); throw std::runtime_error("Line " + std::to_string(peek().line) + ": " + message); }
void Parser::synchronize() { if(is_at_end()) return; advance(); while (!is_at_end()) { if (previous().type == TokenType::SEMICOLON) return; switch (peek().type) { case TokenType::TEXT: case TokenType::STYLE: case TokenType::IDENTIFIER: case TokenType::LBRACKET: case TokenType::HASH: return; default: break; } advance(); } }
