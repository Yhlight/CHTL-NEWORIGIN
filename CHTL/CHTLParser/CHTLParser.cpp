#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens, CHTLContext& context)
    : tokens(tokens), context(context) {}

// --- Core Parsing Logic ---

std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }
    return parseStatement();
}

// --- Token Manipulation Helpers ---

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size();
}

Token CHTLParser::peek() const {
    if (isAtEnd()) return {TokenType::EndOfFile, ""};
    return tokens[current];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message);
}

// --- Statement-Level Parsing ---

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    while (match(TokenType::COMMENT)) {}

    if (check(TokenType::L_BRACKET)) {
        return parseTemplateNode();
    }
    if (check(TokenType::IDENTIFIER)) {
        return parseElementNode();
    }
    if (match(TokenType::KEYWORD_TEXT)) {
        return parseTextNode();
    }

    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    Token tag = consume(TokenType::IDENTIFIER, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tag.value);
    consume(TokenType::L_BRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::KEYWORD_STYLE)) {
            element->setStyle(parseStyleNode());
        } else if (match(TokenType::KEYWORD_SCRIPT)) {
            element->setScript(parseScriptNode());
        } else if (match(TokenType::KEYWORD_TEXT)) {
            element->addChild(parseTextNode());
        } else if (match(TokenType::AT_SIGN)) {
            if (match(TokenType::KEYWORD_ELEMENT)) {
                Token name = consume(TokenType::IDENTIFIER, "Expect template name after @Element.");
                consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
                const TemplateNode* tmpl = context.getTemplate(name.value);
                if (!tmpl || tmpl->getTemplateType() != TemplateType::ELEMENT) {
                    throw std::runtime_error("Attempt to use an undefined or non-element template: " + name.value);
                }
                for (const auto& child : tmpl->getChildren()) {
                    element->addChild(child->clone());
                }
            } else {
                throw std::runtime_error("Unsupported template type in this context.");
            }
        } else if (check(TokenType::IDENTIFIER)) {
            // Lookahead to differentiate between an attribute and a child element.
            if (current + 1 < tokens.size() && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                parseAttribute(element.get());
            } else {
                element->addChild(parseElementNode());
            }
        } else {
            // If we find a token we don't recognize, we can't continue parsing this block.
            throw std::runtime_error("Unexpected token in element block: " + peek().value);
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after element block.");
    return element;
}

void CHTLParser::parseAttribute(ElementNode* element) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute key.");
    if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
        std::string value_str;
        // Attributes can have unquoted literals, so we just consume tokens until the semicolon.
        while (!isAtEnd() && peek().type != TokenType::SEMICOLON) {
            value_str += advance().value;
        }
        if (value_str.empty()) throw std::runtime_error("Expect attribute value.");
        element->addAttribute(key.value, value_str);
        consume(TokenType::SEMICOLON, "Expect ';' after attribute.");
    } else {
        throw std::runtime_error("Expect ':' or '=' after attribute key.");
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseTextNode() {
    consume(TokenType::L_BRACE, "Expect '{' after 'text' keyword.");
    Token text = consume(TokenType::STRING_LITERAL, "Expect string literal inside text block.");
    consume(TokenType::R_BRACE, "Expect '}' after text block.");
    return std::make_unique<TextNode>(text.value);
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleNode() {
    auto styleNode = std::make_unique<StyleNode>();
    consume(TokenType::L_BRACE, "Expect '{' after 'style' keyword.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::AT_SIGN)) {
            if (match(TokenType::KEYWORD_STYLE)) {
                Token name = consume(TokenType::IDENTIFIER, "Expect template name after @Style.");
                consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
                const TemplateNode* tmpl = context.getTemplate(name.value);
                if (!tmpl || tmpl->getTemplateType() != TemplateType::STYLE) {
                    throw std::runtime_error("Attempt to use an undefined or non-style template: " + name.value);
                }
                for (const auto& child : tmpl->getChildren()) {
                    // We expect children of a style template to be StylePropertyNodes
                    if (auto* prop = dynamic_cast<const StylePropertyNode*>(child.get())) {
                        styleNode->addProperty(std::unique_ptr<StylePropertyNode>(static_cast<StylePropertyNode*>(prop->clone().release())));
                    }
                }
            } else {
                throw std::runtime_error("Unsupported template type in this context.");
            }
        } else {
            styleNode->addProperty(parseStyleProperty());
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after style block.");
    return styleNode;
}

std::unique_ptr<StylePropertyNode> CHTLParser::parseStyleProperty() {
    Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
    consume(TokenType::COLON, "Expect ':' after style property key.");
    auto value_expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
    return std::make_unique<StylePropertyNode>(key.value, std::move(value_expr));
}

std::unique_ptr<ScriptNode> CHTLParser::parseScriptNode() {
    consume(TokenType::L_BRACE, "Expect '{' after 'script' keyword.");
    std::string content;
    bool first = true;
    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (!first) {
            if (peek().type != TokenType::SEMICOLON) {
                 content += " ";
            }
        }
        content += advance().value;
        first = false;
    }
    consume(TokenType::R_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content);
}

std::unique_ptr<TemplateNode> CHTLParser::parseTemplateNode() {
    consume(TokenType::L_BRACKET, "Expect '[' at the start of a template definition.");
    consume(TokenType::KEYWORD_TEMPLATE, "Expect 'Template' keyword.");
    consume(TokenType::R_BRACKET, "Expect ']' after 'Template' keyword.");
    consume(TokenType::AT_SIGN, "Expect '@' before template type.");

    TemplateType type;
    if (match(TokenType::KEYWORD_STYLE)) {
        type = TemplateType::STYLE;
    } else if (match(TokenType::KEYWORD_ELEMENT)) {
        type = TemplateType::ELEMENT;
    } else if (match(TokenType::KEYWORD_VAR)) {
        type = TemplateType::VAR;
    } else {
        throw std::runtime_error("Unknown template type.");
    }

    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    auto templateNode = std::make_unique<TemplateNode>(type, name.value);

    consume(TokenType::L_BRACE, "Expect '{' after template name.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (type == TemplateType::STYLE) {
            templateNode->addChild(parseStyleProperty());
        } else {
            templateNode->addChild(parseStatement());
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after template body.");

    context.addTemplate(name.value, std::move(templateNode));
    return nullptr;
}


// --- Expression Parsing (Precedence Climbing) ---

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseTerm();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseFactor() {
    auto expr = parsePower();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        Token op = tokens[current - 1];
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePower() {
    auto expr = parsePrimary();
    if (match(TokenType::STAR_STAR)) {
        Token op = tokens[current - 1];
        auto right = parsePower(); // Right-associative
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        Token num_token = tokens[current - 1];
        // Handle units like 'px', '%', etc.
        if (check(TokenType::IDENTIFIER) || check(TokenType::PERCENT)) {
            Token unit_token = advance();
            Token combined_token = {num_token.type, num_token.value + unit_token.value};
            return std::make_unique<LiteralNode>(combined_token);
        }
        return std::make_unique<LiteralNode>(num_token);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<LiteralNode>(tokens[current - 1]);
    }

    // Handle grouped expressions e.g. (10 + 5)
    if (match(TokenType::L_BRACE)) { // Assuming L_PAREN for grouping, but let's use L_BRACE for now if no L_PAREN exists
        auto expr = parseExpression();
        consume(TokenType::R_BRACE, "Expect ')' after expression.");
        return expr;
    }

    throw std::runtime_error("Expected expression, but found " + peek().value);
}

} // namespace CHTL