#include "Parser.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/OriginNode.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(const std::vector<Token>& t, const std::string& s, CHTLContext& c) : tokens(t), source(s), context(c) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek(size_t offset) const {
    if (position + offset >= tokens.size()) {
        return tokens.back(); // Should be EndOfFile
    }
    return tokens[position + offset];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        position++;
    }
    return tokens[position - 1];
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::consume(TokenType type, const std::string& error_message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(error_message + ". Got " + Token::typeToString(peek().type));
}

std::vector<std::shared_ptr<BaseNode>> Parser::parse() {
    std::vector<std::shared_ptr<BaseNode>> nodes;
    while (!isAtEnd()) {
        if (peek().type == TokenType::LineComment || peek().type == TokenType::BlockComment) {
            advance();
            continue;
        }
        if (peek().type == TokenType::OpenBracket) {
            if (peek(1).type == TokenType::Identifier && peek(2).type == TokenType::CloseBracket) {
                std::string keyword = peek(1).value;
                if (keyword == "Template") {
                    parseTemplateDefinition();
                } else if (keyword == "Custom") {
                    parseCustomDefinition();
                } else if (keyword == "Origin") {
                    nodes.push_back(parseOriginBlock());
                } else {
                    nodes.push_back(parseStatement());
                }
            } else {
                nodes.push_back(parseStatement());
            }
        }
        else {
            nodes.push_back(parseStatement());
        }
    }
    return nodes;
}

std::shared_ptr<OriginNode> Parser::parseOriginBlock() {
    consume(TokenType::OpenBracket, "Expected '['");
    consume(TokenType::Identifier, "Expected 'Origin'");
    consume(TokenType::CloseBracket, "Expected ']'");
    consume(TokenType::At, "Expected '@' after [Origin].");

    Token typeToken = consume(TokenType::Identifier, "Expected origin block type.");

    consume(TokenType::OpenBrace, "Expected '{' after origin type.");

    int brace_level = 1;
    size_t start_pos = peek().char_index;

    while(brace_level > 0 && !isAtEnd()) {
        if (peek().type == TokenType::OpenBrace) {
            brace_level++;
        } else if (peek().type == TokenType::CloseBrace) {
            brace_level--;
        }
        if (brace_level > 0) {
            advance();
        }
    }

    if (brace_level != 0) {
        throw std::runtime_error("Unmatched braces in origin block.");
    }

    // The raw content is from the start position to the current position
    std::string content = source.substr(start_pos, (peek().char_index - start_pos));

    consume(TokenType::CloseBrace, "Expected '}' to close origin block.");

    return std::make_shared<OriginNode>(typeToken.value, content);
}

std::shared_ptr<BaseNode> Parser::parseStatement() {
    if (peek().type == TokenType::Identifier) {
        return parseElementNode();
    }
    if (peek().type == TokenType::Text) {
        return parseTextNode();
    }
    if (peek().type == TokenType::Style){
        return parseStyleNode();
    }
    if (peek().type == TokenType::GeneratorComment) {
        Token commentToken = advance();
        return std::make_shared<CommentNode>(commentToken.value);
    }
    if (peek().type == TokenType::At) {
        return parseUsage();
    }
    throw std::runtime_error("Unexpected token in parseStatement: " + peek().value);
}

std::shared_ptr<ElementNode> Parser::parseElementNode() {
    Token identifier = consume(TokenType::Identifier, "Expected an identifier for element name.");
    auto element = std::make_shared<ElementNode>(identifier.value);

    consume(TokenType::OpenBrace, "Expected '{' after element name.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (peek().type == TokenType::LineComment || peek().type == TokenType::BlockComment) {
            advance();
            continue;
        }

        switch(peek().type) {
            case TokenType::Identifier:
                if (peek(1).type == TokenType::Colon || peek(1).type == TokenType::Equals) {
                    parseAttributes(element);
                } else {
                    element->addChild(parseElementNode());
                }
                break;
            case TokenType::Text:
                element->addChild(parseTextNode());
                break;
            case TokenType::Style:
                element->addChild(parseStyleNode());
                break;
            case TokenType::GeneratorComment:
                element->addChild(std::make_shared<CommentNode>(advance().value));
                break;
            case TokenType::At:
                element->addChild(parseUsage());
                break;
            default:
                 throw std::runtime_error("Unexpected token inside element block: " + peek().value);
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' after element block.");

    return element;
}

void Parser::parseTemplateDefinition() {
    consume(TokenType::OpenBracket, "Expected '['");
    consume(TokenType::Identifier, "Expected 'Template'");
    consume(TokenType::CloseBracket, "Expected ']'");
    consume(TokenType::At, "Expected '@' after [Template].");

    Token typeToken = advance();
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::Style;
    else if (typeToken.value == "Element") type = TemplateType::Element;
    else if (typeToken.value == "Var") type = TemplateType::Var;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);

    Token nameToken = consume(TokenType::Identifier, "Expected template name.");
    auto defNode = std::make_shared<TemplateDefinitionNode>(type, nameToken.value);

    consume(TokenType::OpenBrace, "Expected '{' after template name.");
    while(!check(TokenType::CloseBrace)) {
        defNode->body.push_back(parseStatement());
    }
    consume(TokenType::CloseBrace, "Expected '}' after template body.");

    context.addTemplate(defNode->name, defNode);
}

void Parser::parseCustomDefinition() {
    consume(TokenType::OpenBracket, "Expected '['");
    consume(TokenType::Identifier, "Expected 'Custom'");
    consume(TokenType::CloseBracket, "Expected ']'");
    consume(TokenType::At, "Expected '@' after [Custom].");

    Token typeToken = advance();
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::Style;
    else if (typeToken.value == "Element") type = TemplateType::Element;
    else if (typeToken.value == "Var") type = TemplateType::Var;
    else throw std::runtime_error("Unknown custom type: " + typeToken.value);

    Token nameToken = consume(TokenType::Identifier, "Expected custom name.");
    auto defNode = std::make_shared<CustomDefinitionNode>(type, nameToken.value);

    consume(TokenType::OpenBrace, "Expected '{' after custom name.");
    while(!check(TokenType::CloseBrace)) {
        defNode->body.push_back(parseStatement());
    }
    consume(TokenType::CloseBrace, "Expected '}' after custom body.");

    context.addCustom(defNode->name, defNode);
}

std::shared_ptr<BaseNode> Parser::parseUsage() {
    consume(TokenType::At, "Expected '@' for usage.");
    Token typeToken = advance();
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::Style;
    else if (typeToken.value == "Element") type = TemplateType::Element;
    else if (typeToken.value == "Var") type = TemplateType::Var;
    else throw std::runtime_error("Unknown usage type: " + typeToken.value);

    Token nameToken = consume(TokenType::Identifier, "Expected name for usage.");

    if (peek().type == TokenType::Semicolon) {
        advance(); // consume ';'
        return std::make_shared<TemplateUsageNode>(type, nameToken.value);
    }

    if (peek().type == TokenType::OpenBrace) {
        auto usageNode = std::make_shared<CustomUsageNode>(type, nameToken.value);
        consume(TokenType::OpenBrace, "Expected '{' for custom usage body.");
        while(!check(TokenType::CloseBrace)) {
            // For now, parse the specialization body as generic statements
            usageNode->specializationBody.push_back(parseStatement());
        }
        consume(TokenType::CloseBrace, "Expected '}' after custom usage body.");
        return usageNode;
    }

    throw std::runtime_error("Expected ';' or '{' after usage name.");
}

std::shared_ptr<StyleNode> Parser::parseStyleNode() {
    consume(TokenType::Style, "Expected 'style' keyword.");
    consume(TokenType::OpenBrace, "Expected '{' after 'style' keyword.");

    auto styleNode = std::make_shared<StyleNode>();
    StyleRule inlineRule;
    inlineRule.selector = ""; // For inline styles

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (peek().type == TokenType::Dot || peek().type == TokenType::Hash || peek().type == TokenType::Ampersand) {
            // This is a selector rule
            StyleRule rule;
            std::string selector_str;
            while(peek().type != TokenType::OpenBrace && !isAtEnd()) {
                selector_str += advance().value;
            }
            rule.selector = selector_str;

            consume(TokenType::OpenBrace, "Expected '{' after selector.");
            while(!check(TokenType::CloseBrace)) {
                Token key = consume(TokenType::Identifier, "Expected style property key.");
                consume(TokenType::Colon, "Expected ':' after style property key.");

                std::string value_str;
                while(peek().type != TokenType::Semicolon && peek().type != TokenType::CloseBrace) {
                    value_str += advance().value + " ";
                }
                if (!value_str.empty()) value_str.pop_back(); // remove trailing space
                rule.properties[key.value] = value_str;

                consume(TokenType::Semicolon, "Expected ';' after style property.");
            }
            consume(TokenType::CloseBrace, "Expected '}' after rule block.");
            styleNode->rules.push_back(rule);

        } else if (peek().type == TokenType::Identifier) {
            // This is an inline property
            Token key = consume(TokenType::Identifier, "Expected style property key.");
            consume(TokenType::Colon, "Expected ':' after style property key.");

            std::string value_str;
            while(peek().type != TokenType::Semicolon && peek().type != TokenType::CloseBrace) {
                value_str += advance().value + " ";
            }
            if (!value_str.empty()) value_str.pop_back(); // remove trailing space
            inlineRule.properties[key.value] = value_str;

            consume(TokenType::Semicolon, "Expected ';' after style property.");
        } else if (peek().type == TokenType::At) {
            styleNode->children.push_back(parseUsage());
        }
        else {
            throw std::runtime_error("Unexpected token in style block: " + peek().value);
        }
    }

    if (!inlineRule.properties.empty()) {
        styleNode->rules.push_back(inlineRule);
    }

    consume(TokenType::CloseBrace, "Expected '}' after style block.");
    return styleNode;
}

std::shared_ptr<TextNode> Parser::parseTextNode() {
    consume(TokenType::Text, "Expected 'text' keyword.");
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");

    // Text can be a string literal or an unquoted literal
    Token content = advance();
    if (content.type != TokenType::StringLiteral && content.type != TokenType::UnquotedLiteral) {
        throw std::runtime_error("Expected a string or unquoted literal for text content.");
    }

    consume(TokenType::CloseBrace, "Expected '}' after text content.");

    return std::make_shared<TextNode>(content.value);
}

void Parser::parseAttributes(std::shared_ptr<ElementNode> node) {
    Token key = consume(TokenType::Identifier, "Expected attribute key.");

    // Handle CE Equivalence (Colon = Equal)
    if (peek().type == TokenType::Colon || peek().type == TokenType::Equals) {
        advance();
    } else {
        throw std::runtime_error("Expected ':' or '=' after attribute key.");
    }

    Token value = advance();
    if (value.type != TokenType::StringLiteral && value.type != TokenType::UnquotedLiteral && value.type != TokenType::Identifier) {
        throw std::runtime_error("Expected a string, unquoted literal, or identifier for attribute value.");
    }

    node->setAttribute(key.value, value.value);

    consume(TokenType::Semicolon, "Expected ';' after attribute value.");
}

} // namespace CHTL
