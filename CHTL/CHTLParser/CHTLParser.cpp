#include "CHTLParser.h"

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

Token CHTLParser::peek() {
    if (isAtEnd()) {
        return Token{TokenType::TOKEN_EOF, "", 0, 0};
    }
    return tokens[position];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        position++;
    }
    return tokens[position - 1];
}

bool CHTLParser::isAtEnd() {
    return position >= tokens.size();
}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    // For now, we'll assume the root is a single element.
    // This will be expanded to handle a full CHTL document.
    if (peek().type == TokenType::TOKEN_IDENTIFIER) {
        return parseElement();
    }
    // Return a null pointer if parsing fails or the input is empty
    return nullptr;
}

std::shared_ptr<ElementNode> CHTLParser::parseElement() {
    Token identifier = advance(); // Consume the element tag
    auto element = std::make_shared<ElementNode>(identifier.lexeme);

    if (peek().type == TokenType::TOKEN_LBRACE) {
        advance(); // Consume '{'

        while (peek().type != TokenType::TOKEN_RBRACE && !isAtEnd()) {
            if (peek().type == TokenType::TOKEN_IDENTIFIER) {
                // Check if it's an attribute (identifier followed by a colon)
                if (tokens[position + 1].type == TokenType::TOKEN_COLON || tokens[position + 1].type == TokenType::TOKEN_ASSIGN) {
                    parseAttributes(element);
                } else {
                    // It's a nested element
                    element->addChild(parseElement());
                }
            } else if (peek().lexeme == "text") {
                element->addChild(parseText());
            } else {
                // Handle other node types or syntax errors
                advance();
            }
        }

        if (peek().type == TokenType::TOKEN_RBRACE) {
            advance(); // Consume '}'
        } else {
            // Error: unclosed element
        }
    }

    return element;
}

std::shared_ptr<TextNode> CHTLParser::parseText() {
    advance(); // consume 'text' keyword
    if (peek().type == TokenType::TOKEN_LBRACE) {
        advance(); // consume '{'
        std::string content_text;
        if (peek().type == TokenType::TOKEN_STRING_LITERAL || peek().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
            Token content = advance();
            content_text = content.lexeme;
        }
        if (peek().type == TokenType::TOKEN_RBRACE) {
            advance(); // consume '}'
        } else {
            // Error: unclosed text block
        }
        return std::make_shared<TextNode>(content_text);
    }
    return nullptr;
}


void CHTLParser::parseAttributes(std::shared_ptr<ElementNode> element) {
    while (peek().type == TokenType::TOKEN_IDENTIFIER) {
        std::string key = advance().lexeme;
        if (peek().type == TokenType::TOKEN_COLON || peek().type == TokenType::TOKEN_ASSIGN) {
            advance(); // consume ':' or '='
            if (peek().type == TokenType::TOKEN_STRING_LITERAL || peek().type == TokenType::TOKEN_UNQUOTED_LITERAL || peek().type == TokenType::TOKEN_NUMERIC_LITERAL) {
                std::string value = advance().lexeme;
                element->setAttribute(key, value);
                if (peek().type == TokenType::TOKEN_SEMICOLON) {
                    advance(); // consume ';'
                }
            } else {
                // Error: unexpected token in attribute value
            }
        } else {
            // Error: expected ':' or '=' after attribute key
            // We need to put the token back.
            position--;
            break;
        }
    }
}

}