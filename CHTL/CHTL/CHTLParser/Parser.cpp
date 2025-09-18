#include "Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    }
    m_errors.push_back("Error: Expected next token to be " + std::to_string(static_cast<int>(t)) +
                       ", but got " + std::to_string(static_cast<int>(m_peekToken.type)) + " instead.");
    return false;
}

NodePtr Parser::parseProgram() {
    auto programNode = std::make_shared<ElementNode>(Token{TokenType::IDENTIFIER, "program", 0}, "program");

    while (m_curToken.type != TokenType::END_OF_FILE) {
        NodePtr stmt = parseStatement();
        if (stmt) {
            programNode->m_children.push_back(stmt);
        }
        nextToken();
    }
    return programNode;
}

NodePtr Parser::parseStatement() {
    switch (m_curToken.type) {
        case TokenType::IDENTIFIER:
            return parseElementStatement();
        case TokenType::TEXT:
            return parseTextStatement();
        case TokenType::GENERATOR_COMMENT:
            return parseCommentStatement();
        default:
            return nullptr;
    }
}

NodePtr Parser::parseCommentStatement() {
    return std::make_shared<CommentNode>(m_curToken, m_curToken.literal);
}

void Parser::parseAttribute(std::shared_ptr<ElementNode> node) {
    // Current token is IDENTIFIER (the attribute key)
    std::string key = m_curToken.literal;

    // Advance past the key to the ':' or '='
    nextToken();

    // Advance past ':' or '=' to the value
    nextToken();

    std::string value;
    if (m_curToken.type == TokenType::STRING || m_curToken.type == TokenType::IDENTIFIER) {
        value = m_curToken.literal;
    } else {
        m_errors.push_back("Attribute value for key '" + key + "' must be a string or an identifier.");
        return;
    }

    node->m_attributes[key] = value;

    if (m_peekToken.type != TokenType::SEMICOLON) {
        m_errors.push_back("Expected ';' after attribute value for key '" + key + "'.");
        return; // Let's not advance so the main loop can see the problem token
    }
    nextToken(); // Consume the value
    // curToken is now ';'
}

NodePtr Parser::parseElementStatement() {
    auto node = std::make_shared<ElementNode>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // Consume '{'

    while (m_curToken.type != TokenType::RIGHT_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        if (m_curToken.type == TokenType::TEXT) {
            NodePtr child = parseTextStatement();
            if (child) {
                node->m_children.push_back(child);
            }
        } else if (m_curToken.type == TokenType::IDENTIFIER) {
            if (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUALS) {
                parseAttribute(node);
            } else if (m_peekToken.type == TokenType::LEFT_BRACE) {
                NodePtr child = parseElementStatement();
                if (child) {
                    node->m_children.push_back(child);
                }
            } else {
                m_errors.push_back("Unexpected token '" + m_peekToken.literal + "' after identifier '" + m_curToken.literal + "'.");
            }
        } else if (m_curToken.type == TokenType::GENERATOR_COMMENT) {
            NodePtr child = parseCommentStatement();
            if (child) {
                node->m_children.push_back(child);
            }
        } else {
            m_errors.push_back("Unexpected token in element block: '" + m_curToken.literal + "'.");
            // To prevent infinite loops, advance the token
            nextToken();
            continue;
        }

        // Advance to the next statement/attribute/token
        nextToken();
    }

    return node;
}

NodePtr Parser::parseTextStatement() {
    auto textElementNode = std::make_shared<ElementNode>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }

    if (!expectPeek(TokenType::STRING)) {
        return nullptr;
    }

    auto textValueNode = std::make_shared<TextNode>(m_curToken, m_curToken.literal);
    textElementNode->m_children.push_back(textValueNode);

    if (!expectPeek(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }

    return textElementNode;
}
