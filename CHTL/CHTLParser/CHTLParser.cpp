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

    // Check for a text block first, as it's more specific.
    if (check(TokenType::TEXT_KEYWORD)) {
        // A text block is specifically `text { STRING_LITERAL }`.
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::L_BRACE &&
            current + 2 < tokens.size() && tokens[current + 2].type == TokenType::STRING_LITERAL &&
            current + 3 < tokens.size() && tokens[current + 3].type == TokenType::R_BRACE)
        {
            advance(); // Consume TEXT_KEYWORD
            return parseTextNode();
        }
    }

    // Otherwise, try to parse an element. This handles `div {}` and `text {}` (as an element).
    if (check(TokenType::IDENTIFIER) || check(TokenType::TEXT_KEYWORD)) {
        return parseElementNode();
    }

    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    Token tag;
    if (check(TokenType::IDENTIFIER) || check(TokenType::TEXT_KEYWORD)) {
        tag = advance();
    } else {
        // Will throw error with correct message
        consume(TokenType::IDENTIFIER, "Expect element tag name.");
    }
    auto element = std::make_unique<ElementNode>(tag.value);
    consume(TokenType::L_BRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::STYLE_KEYWORD)) {
            element->setStyle(parseStyleNode());
        } else if (match(TokenType::SCRIPT_KEYWORD)) {
            element->setScript(parseScriptNode());
        } else if (check(TokenType::TEXT_KEYWORD)) { // Use check() to peek
            // Look ahead to see if it's an attribute `text:` or a block `text {`
            if (current + 1 < tokens.size() && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                // It's a text attribute.
                advance(); // Consume TEXT_KEYWORD
                advance(); // Consume COLON or EQUAL

                std::string value_str;
                // Text attributes can have unquoted literals, so we just consume tokens until the semicolon.
                while (!isAtEnd() && peek().type != TokenType::SEMICOLON) {
                    value_str += advance().value;
                }

                if (value_str.empty()) {
                    throw std::runtime_error("Expect attribute value for text.");
                }

                consume(TokenType::SEMICOLON, "Expect ';' after text attribute.");
                element->addChild(std::make_unique<TextNode>(value_str));
            } else {
                // It's a text block OR a text element. Look ahead to distinguish.
                // A text block is `text { STRING_LITERAL }`.
                if (current + 2 < tokens.size() &&
                    tokens[current + 2].type == TokenType::STRING_LITERAL &&
                    current + 3 < tokens.size() &&
                    tokens[current + 3].type == TokenType::R_BRACE) {
                    // It's a text block: text { "..." }
                    advance(); // Consume TEXT_KEYWORD
                    element->addChild(parseTextNode());
                } else {
                    // It's a text element.
                    element->addChild(parseElementNode());
                }
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
        Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
        consume(TokenType::COLON, "Expect ':' after style property key.");
        auto value_expr = parseExpression();
        styleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, std::move(value_expr)));
        consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
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
    if (match(TokenType::NUMBER) || match(TokenType::DIMENSION)) {
        return std::make_unique<LiteralNode>(tokens[current - 1]);
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