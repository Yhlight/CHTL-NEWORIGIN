#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }
    return parseStatement();
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

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size();
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

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    while (match(TokenType::COMMENT)) {
        // Skip comments
    }

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
            element->setStyle(parseStyleNode());
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
        while (peek().type != TokenType::SEMICOLON && !isAtEnd()) {
            value_str += advance().value;
        }
        if (value_str.empty()) {
            throw std::runtime_error("Expect attribute value.");
        }
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

        std::string value_str;
        while (peek().type != TokenType::SEMICOLON && !isAtEnd()) {
            value_str += advance().value;
        }

        if (value_str.empty()) {
            throw std::runtime_error("Expect style property value.");
        }

        styleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, value_str));
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

} // namespace CHTL