#include "CHTLParser.h"
#include "../CHTLNode/CommentNode.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {
    m_eofToken.type = TokenType::END_OF_FILE;
    if (!m_tokens.empty()) {
        m_eofToken.line = m_tokens.back().line;
        m_eofToken.column = m_tokens.back().column + 1;
    }
}

const Token& CHTLParser::currentToken() const {
    if (m_position >= m_tokens.size()) return m_eofToken;
    return m_tokens[m_position];
}

const Token& CHTLParser::peekToken() const {
    if (m_position + 1 >= m_tokens.size()) return m_eofToken;
    return m_tokens[m_position + 1];
}

void CHTLParser::advance() {
    if (m_position < m_tokens.size()) m_position++;
}

bool CHTLParser::match(TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

const Token& CHTLParser::consume(TokenType type, const std::string& errorMessage) {
    const auto& token = currentToken();
    if (token.type == type) {
        advance();
        return m_tokens[m_position - 1];
    }
    throw std::runtime_error(errorMessage + " Got '" + token.value + "' instead. Line: " + std::to_string(token.line) + ", Col: " + std::to_string(token.column));
}

std::unique_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "document_root";
    while (currentToken().type != TokenType::END_OF_FILE) {
        root->children.push_back(parseNode());
    }
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseNode() {
    if (currentToken().type == TokenType::GENERATOR_COMMENT) {
        auto commentNode = std::make_unique<CommentNode>();
        commentNode->type = CommentType::GENERATOR;
        commentNode->content = currentToken().value;
        advance();
        return commentNode;
    }
    if (currentToken().type == TokenType::IDENTIFIER) {
        return parseElement();
    }
    if (currentToken().type == TokenType::KEYWORD_TEXT) {
        return parseTextNode();
    }

    const auto& token = currentToken();
    throw std::runtime_error("Unexpected token while parsing node: '" + token.value + "' at Line: " + std::to_string(token.line));
}

Attribute CHTLParser::parseAttribute() {
    Attribute attr;
    attr.key = consume(TokenType::IDENTIFIER, "Expected attribute key.").value;

    if (!match(TokenType::COLON) && !match(TokenType::EQUALS)) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + attr.key + "'.");
    }

    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
        attr.value = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected attribute value for key '" + attr.key + "'.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value for key '" + attr.key + "'.");
    return attr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expected element tag name.").value;

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name '" + element->tagName + "'.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::GENERATOR_COMMENT) {
            element->children.push_back(parseNode());
        } else if (currentToken().type == TokenType::IDENTIFIER && (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS)) {
            element->attributes.push_back(parseAttribute());
        } else {
            element->children.push_back(parseNode());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close element '" + element->tagName + "'.");

    return element;
}

std::unique_ptr<TextNode> CHTLParser::parseTextNode() {
    consume(TokenType::KEYWORD_TEXT, "Expected 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text' keyword.");

    auto textNode = std::make_unique<TextNode>();

    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::UNQUOTED_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
        textNode->content = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected string or unquoted literal inside text block.");
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close text block.");

    return textNode;
}

} // namespace CHTL
