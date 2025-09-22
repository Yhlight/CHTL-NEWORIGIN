#include "Parser.h"
#include <utility>

namespace CHTL {

Parser::Parser(Lexer& lexer) : m_lexer(lexer), m_currentToken(TokenType::Illegal, ""), m_peekToken(TokenType::Illegal, "") {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.NextToken();
}

const std::vector<std::string>& Parser::Errors() const {
    return m_errors;
}

void Parser::peekError(TokenType type) {
    std::string error = "Expected next token to be of type " + std::to_string((int)type) +
                        ", but got " + std::to_string((int)m_peekToken.type) + " instead.";
    m_errors.push_back(error);
}

bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    }
    peekError(type);
    return false;
}

bool Parser::currentTokenIs(TokenType type) const {
    return m_currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) const {
    return m_peekToken.type == type;
}

// The main entry point. Creates a root node to hold all top-level statements.
std::unique_ptr<ElementNode> Parser::ParseProgram() {
    auto root = std::make_unique<ElementNode>("__ROOT__");
    while (!currentTokenIs(TokenType::Eof)) {
        auto stmt = parseStatement();
        if (stmt) {
            root->children.push_back(std::move(stmt));
        } else {
            // An error occurred and parseStatement returned nullptr.
            // The token is unparseable. Skip it to avoid an infinite loop.
            m_errors.push_back("Skipping unparseable token: " + m_currentToken.literal);
        }

        if (currentTokenIs(TokenType::Eof)) break;

        // After parsing a statement, we must advance to the next token.
        // `parseStatement` leaves us on the LAST token of the statement (e.g. '}')
        // so we need to advance to get to the start of the next one.
        nextToken();
    }
    return root;
}

// Dispatches to the correct parsing function based on the current token.
std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (currentTokenIs(TokenType::Identifier)) {
        if (m_currentToken.literal == "text") {
            return parseTextNode();
        }
        return parseElementNode();
    }
    m_errors.push_back("Unexpected token, expected a statement (e.g., an element tag).");
    return nullptr;
}

// Parses a complete element, including its attributes and children.
// Assumes m_currentToken is the element's tag (Identifier).
std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto element = std::make_unique<ElementNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::LBrace)) return nullptr;

    while (!peekTokenIs(TokenType::RBrace) && !peekTokenIs(TokenType::Eof)) {
        nextToken(); // Consume '{' or the last token of the previous statement.

        if (!currentTokenIs(TokenType::Identifier)) {
             m_errors.push_back("Unexpected token inside element block.");
             continue; // Skip to the next token
        }

        if (peekTokenIs(TokenType::Colon) || peekTokenIs(TokenType::Equals)) {
            auto attr = parseAttributeNode();
            if (attr) {
                element->attributes.push_back(std::move(attr));
            }
        } else {
            auto child = parseStatement();
            if (child) {
                element->children.push_back(std::move(child));
            }
        }
    }

    if (!expectPeek(TokenType::RBrace)) return nullptr;

    return element;
}

// Parses a single attribute from name to semicolon.
// Assumes m_currentToken is the attribute's name (Identifier).
std::unique_ptr<AttributeNode> Parser::parseAttributeNode() {
    std::string name = m_currentToken.literal;

    if (!peekTokenIs(TokenType::Colon) && !peekTokenIs(TokenType::Equals)) {
        m_errors.push_back("Expected ':' or '=' after attribute name.");
        return nullptr;
    }
    nextToken(); // Consume name, currentToken is now ':' or '='

    nextToken(); // Consume ':', currentToken is now the value

    if (!currentTokenIs(TokenType::Identifier) && !currentTokenIs(TokenType::String)) {
        m_errors.push_back("Invalid attribute value. Must be an identifier or a string.");
        return nullptr;
    }
    std::string value = m_currentToken.literal;

    auto attr = std::make_unique<AttributeNode>(name, value);

    if (!expectPeek(TokenType::Semicolon)) return nullptr;

    return attr;
}

// Parses a text block from `text` to `}`.
// Assumes m_currentToken is the 'text' identifier.
std::unique_ptr<TextNode> Parser::parseTextNode() {
    if (!expectPeek(TokenType::LBrace)) return nullptr;
    if (!expectPeek(TokenType::String)) return nullptr;

    auto node = std::make_unique<TextNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::RBrace)) return nullptr;

    return node;
}

} // CHTL
