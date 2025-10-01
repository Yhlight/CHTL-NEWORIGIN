#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/PropertyAccessNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Core Parsing Logic ---

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseProgram() {
    std::vector<std::unique_ptr<BaseNode>> program;
    while (!isAtEnd()) {
        program.push_back(parseStatement());
        while (match(TokenType::COMMENT)) {}
    }
    return program;
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
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::IDENTIFIER) {
            const std::string& keyword = tokens[current + 1].value;
            if (keyword == "Template") {
                return parseTemplateNode();
            } else if (keyword == "Origin") {
                return parseOriginNode();
            }
        }
    }

    if (check(TokenType::IDENTIFIER)) {
        return parseElementNode();
    }
    if (match(TokenType::TEXT_KEYWORD)) {
        return parseTextNode();
    }

    if (isAtEnd()) return nullptr;
    throw std::runtime_error("Unexpected token at top level: " + peek().value);
}

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    Token tag = consume(TokenType::IDENTIFIER, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tag.value);
    consume(TokenType::L_BRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::STYLE_KEYWORD)) {
            element->setStyle(parseStyleNode(element.get()));
        } else if (match(TokenType::SCRIPT_KEYWORD)) {
            element->setScript(parseScriptNode());
        } else if (check(TokenType::AT)) {
            consume(TokenType::AT, "Expect '@' for template usage.");
            Token templateType = consume(TokenType::IDENTIFIER, "Expect template type.");
            Token templateName = consume(TokenType::IDENTIFIER, "Expect template name.");
            consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
            element->addChild(std::make_unique<TemplateUsageNode>(templateType.value, templateName.value));
        } else if (check(TokenType::L_BRACKET)) {
            if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::IDENTIFIER && tokens[current + 1].value == "Origin") {
                 element->addChild(parseOriginNode());
            } else {
                 throw std::runtime_error("Unexpected block type inside element.");
            }
        } else if (check(TokenType::TEXT_KEYWORD)) { // Use check() to peek
            // Look ahead to see if it's an attribute `text:` or a block `text {`
            if (current + 1 < tokens.size() && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                // It's a text attribute.
                advance(); // Consume TEXT_KEYWORD
                advance(); // Consume COLON or EQUAL

                Token value;
                if (match(TokenType::STRING_LITERAL) || match(TokenType::IDENTIFIER)) {
                    value = tokens[current - 1];
                } else {
                    throw std::runtime_error("Expect string or literal for text attribute value.");
                }

                consume(TokenType::SEMICOLON, "Expect ';' after text attribute.");
                element->addChild(std::make_unique<TextNode>(value.value));
            } else {
                // It's a text block.
                advance(); // Consume TEXT_KEYWORD
                element->addChild(parseTextNode());
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

std::unique_ptr<StyleNode> CHTLParser::parseStyleNode(ElementNode* parent) {
    auto styleNode = std::make_unique<StyleNode>();
    consume(TokenType::L_BRACE, "Expect '{' after 'style' keyword.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (check(TokenType::AT)) {
            consume(TokenType::AT, "Expect '@' for template usage.");
            Token templateType = consume(TokenType::IDENTIFIER, "Expect template type.");
            Token templateName = consume(TokenType::IDENTIFIER, "Expect template name.");
            consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
            styleNode->addTemplateUsage(std::make_unique<TemplateUsageNode>(templateType.value, templateName.value));
        } else if (check(TokenType::DOT) || check(TokenType::HASH) || check(TokenType::AMPERSAND)) { // It's a style rule
            std::string full_selector;

            if (match(TokenType::AMPERSAND)) {
                std::string baseSelector;
                std::string classAttr = parent->getAttribute("class");
                if (!classAttr.empty()) {
                    baseSelector = "." + classAttr;
                } else {
                    std::string idAttr = parent->getAttribute("id");
                    if (!idAttr.empty()) {
                        baseSelector = "#" + idAttr;
                    } else {
                        throw std::runtime_error("Contextual selector '&' used without a parent class or id.");
                    }
                }

                std::string selectorSuffix;
                while(!isAtEnd() && !check(TokenType::L_BRACE)) {
                    selectorSuffix += advance().value;
                }
                full_selector = baseSelector + selectorSuffix;
            } else { // It's a .class or #id selector
                 Token selector_start = advance(); // Consume '.' or '#'
                 Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name.");
                 full_selector = selector_start.value + selector_name.value;

                 // Automatically add attribute to parent element
                 if (selector_start.type == TokenType::DOT) {
                     parent->addAttribute("class", selector_name.value);
                 } else { // HASH
                     parent->addAttribute("id", selector_name.value);
                 }
            }

            auto ruleNode = std::make_unique<StyleRuleNode>(full_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector.");

            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                // Parse properties inside the rule
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                auto value_expr = parseExpression();
                ruleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, std::move(value_expr)));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }

            consume(TokenType::R_BRACE, "Expect '}' after style rule block.");
            styleNode->addRule(std::move(ruleNode));

        } else if (check(TokenType::IDENTIFIER)) { // It's an inline property
            Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
            consume(TokenType::COLON, "Expect ':' after style property key.");
            auto value_expr = parseExpression();
            styleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, std::move(value_expr)));
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else {
            throw std::runtime_error("Unexpected token in style block: " + peek().value);
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after style block.");
    return styleNode;
}

std::unique_ptr<ScriptNode> CHTLParser::parseScriptNode() {
    consume(TokenType::L_BRACE, "Expect '{' after 'script' keyword.");
    Token content = consume(TokenType::STRING_LITERAL, "Expect script content.");
    consume(TokenType::R_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content.value);
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginNode() {
    consume(TokenType::L_BRACKET, "Expect '[' to start origin block.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Origin' keyword.");
    if (keyword.value != "Origin") {
        throw std::runtime_error("Expected 'Origin' keyword, but found " + keyword.value);
    }
    consume(TokenType::R_BRACKET, "Expect ']' to end origin block tag.");
    consume(TokenType::AT, "Expect '@' before origin type.");
    Token type = consume(TokenType::IDENTIFIER, "Expect origin type.");
    consume(TokenType::L_BRACE, "Expect '{' after origin type.");

    // The lexer is expected to provide the content as a single string literal
    Token content = consume(TokenType::STRING_LITERAL, "Expect origin content.");

    consume(TokenType::R_BRACE, "Expect '}' after origin content.");

    return std::make_unique<OriginNode>(type.value, content.value);
}

std::unique_ptr<TemplateNode> CHTLParser::parseTemplateNode() {
    consume(TokenType::L_BRACKET, "Expect '[' to start template block.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Template' keyword.");
    if (keyword.value != "Template") {
        throw std::runtime_error("Expected 'Template' keyword, but found " + keyword.value);
    }
    consume(TokenType::R_BRACKET, "Expect ']' to end template block tag.");
    consume(TokenType::AT, "Expect '@' before template type.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");

    auto templateNode = std::make_unique<TemplateNode>(type.value, name.value);

    consume(TokenType::L_BRACE, "Expect '{' after template name.");

    if (type.value == "Style") {
        while (!check(TokenType::R_BRACE) && !isAtEnd()) {
            Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
            consume(TokenType::COLON, "Expect ':' after style property key.");
            auto value_expr = parseExpression();
            templateNode->addProperty(std::make_unique<StylePropertyNode>(key.value, std::move(value_expr)));
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        }
    } else if (type.value == "Element") {
        while (!check(TokenType::R_BRACE) && !isAtEnd()) {
            templateNode->addChild(parseStatement());
        }
    } else {
        throw std::runtime_error("Unknown template type: " + type.value);
    }

    consume(TokenType::R_BRACE, "Expect '}' after template block.");
    return templateNode;
}

// --- Expression Parsing (Precedence Climbing) ---

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseTernary();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTernary() {
    auto condition = parseLogicalOr();

    if (match(TokenType::QUESTION)) {
        auto trueExpr = parseTernary(); // Allow nested ternaries
        consume(TokenType::COLON, "Expect ':' after true expression in ternary operator.");
        auto falseExpr = parseTernary();
        return std::make_unique<ConditionalNode>(std::move(condition), std::move(trueExpr), std::move(falseExpr));
    }

    return condition;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::LOGICAL_OR)) {
        Token op = tokens[current - 1];
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalAnd() {
    auto expr = parseRelational();
    while (match(TokenType::LOGICAL_AND)) {
        Token op = tokens[current - 1];
        auto right = parseRelational();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseRelational() {
    auto expr = parseTerm();
    while (match(TokenType::GREATER_THAN) || match(TokenType::LESS_THAN)) {
        Token op = tokens[current - 1];
        auto right = parseTerm();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
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
    if (match(TokenType::NUMBER) || match(TokenType::DIMENSION) || match(TokenType::STRING_LITERAL)) {
        return std::make_unique<LiteralNode>(tokens[current - 1]);
    }

    // Check for property access: selector.property
    bool isPropAccess = false;
    if (check(TokenType::HASH) || check(TokenType::DOT)) {
        if (current + 2 < tokens.size() && tokens[current + 2].type == TokenType::DOT) {
            isPropAccess = true;
        }
    } else if (check(TokenType::IDENTIFIER)) {
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::DOT) {
            isPropAccess = true;
        }
    }

    if (isPropAccess) {
        std::string selector_str;
        if (match(TokenType::HASH) || match(TokenType::DOT)) {
            Token selector_start = tokens[current - 1];
            Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name after # or .");
            selector_str = selector_start.value + selector_name.value;
        } else { // IDENTIFIER
            selector_str = advance().value;
        }

        consume(TokenType::DOT, "Expect '.' for property access.");
        Token prop_name = consume(TokenType::IDENTIFIER, "Expect property name after '.'");
        return std::make_unique<PropertyAccessNode>(selector_str, prop_name.value);
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