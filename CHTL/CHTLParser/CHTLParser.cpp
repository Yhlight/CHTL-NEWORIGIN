#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/SelectorBlockNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

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

    if (check(TokenType::IDENTIFIER)) {
        return parseElementNode();
    }
    if (match(TokenType::TEXT_KEYWORD)) {
        return parseTextNode();
    }

    return nullptr;
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
        } else if (match(TokenType::TEXT_KEYWORD)) {
            element->addChild(parseTextNode());
        } else if (check(TokenType::IDENTIFIER)) {
            if (current + 1 < tokens.size() && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                parseAttribute(element.get());
            } else {
                element->addChild(parseElementNode());
            }
        } else {
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

    std::string last_selector;

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::DOT)) {
            Token name = consume(TokenType::IDENTIFIER, "Expect class name after '.'.");
            last_selector = "." + name.value;
            parent->addAttribute("class", name.value);
            auto selectorBlock = std::make_unique<SelectorBlockNode>(last_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector name.");
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        } else if (match(TokenType::HASH)) {
            Token name = consume(TokenType::IDENTIFIER, "Expect id name after '#'.");
            last_selector = "#" + name.value;
            parent->addAttribute("id", name.value);
            auto selectorBlock = std::make_unique<SelectorBlockNode>(last_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector name.");
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        } else if (match(TokenType::AMPERSAND)) {
            if (last_selector.empty()) throw std::runtime_error("Found '&' without a preceding selector context.");
            std::string pseudo_selector_part;
            while (!check(TokenType::L_BRACE) && !isAtEnd()) {
                pseudo_selector_part += advance().value;
            }
            std::string full_selector = last_selector + pseudo_selector_part;
            auto selectorBlock = std::make_unique<SelectorBlockNode>(full_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector.");
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        }
        else if (check(TokenType::IDENTIFIER)) {
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

// --- Expression Parsing (Precedence Climbing) ---

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseTernary();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTernary() {
    auto expr = parseLogicalOr();
    if (match(TokenType::QUESTION_MARK)) {
        auto true_expr = parseExpression();
        consume(TokenType::COLON, "Expect ':' after true expression in ternary operator.");
        auto false_expr = parseExpression();
        return std::make_unique<ConditionalExpressionNode>(std::move(expr), std::move(true_expr), std::move(false_expr));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::PIPE_PIPE)) {
        Token op = tokens[current - 1];
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AMPERSAND_AMPERSAND)) {
        Token op = tokens[current - 1];
        auto right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        Token op = tokens[current - 1];
        auto right = parseComparison();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
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
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        Token num_token = tokens[current - 1];
        std::string unit;
        if (check(TokenType::IDENTIFIER)) {
            unit = advance().value;
        } else if (check(TokenType::PERCENT)) {
            size_t next_token_idx = current + 1;
            if (next_token_idx >= tokens.size() ||
                (tokens[next_token_idx].type != TokenType::NUMBER &&
                 tokens[next_token_idx].type != TokenType::IDENTIFIER &&
                 tokens[next_token_idx].type != TokenType::L_BRACE)) {
                unit = advance().value;
            }
        }
        return std::make_unique<LiteralNode>(num_token, unit);
    }

    if (match(TokenType::IDENTIFIER) || match(TokenType::STRING_LITERAL)) {
        return std::make_unique<LiteralNode>(tokens[current - 1], "");
    }

    if (match(TokenType::L_BRACE)) {
        auto expr = parseExpression();
        consume(TokenType::R_BRACE, "Expect '}' after expression.");
        return expr;
    }

    throw std::runtime_error("Expected expression, but found " + peek().value);
}

} // namespace CHTL