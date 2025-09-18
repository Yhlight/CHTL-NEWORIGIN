#include "Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    // Prime the parser by loading the first two tokens.
    // m_curToken and m_peekToken will be set.
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
    // A CHTL program is a series of statements. For now, we'll parse them
    // and create a single root "program" node to hold them.
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
        default:
            return nullptr;
    }
}

std::vector<NodePtr> Parser::parseBlock() {
    std::vector<NodePtr> statements;

    // The opening '{' is consumed by the caller.
    // We parse until we find the closing '}'.
    while (m_curToken.type != TokenType::RIGHT_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        NodePtr stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        } else {
            // If parseStatement returns null, it's an unexpected token.
            m_errors.push_back("Error: Unexpected token '" + m_curToken.literal + "' on line " + std::to_string(m_curToken.line));
        }
        nextToken();
    }
    // The loop terminates with m_curToken being '}' or EOF. The caller will handle it.
    return statements;
}

NodePtr Parser::parseElementStatement() {
    auto node = std::make_shared<ElementNode>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr; // Error already logged by expectPeek
    }

    // Consume the '{' and move to the first token of the block.
    nextToken();

    node->m_children = parseBlock();

    // After parseBlock, m_curToken is the '}'. We don't need to consume it here,
    // as the main loop in parseProgram will call nextToken().

    return node;
}

NodePtr Parser::parseTextStatement() {
    // The current token is 'text'. Create an ElementNode for it.
    auto textElementNode = std::make_shared<ElementNode>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    // m_curToken is now '{'

    if (!expectPeek(TokenType::STRING)) {
        return nullptr;
    }
    // m_curToken is now the STRING token

    // The content of the text element is a TextNode.
    auto textValueNode = std::make_shared<TextNode>(m_curToken, m_curToken.literal);
    textElementNode->m_children.push_back(textValueNode);

    if (!expectPeek(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }
    // m_curToken is now '}'

    return textElementNode;
}
