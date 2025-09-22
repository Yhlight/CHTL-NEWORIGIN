#include "Parser.h"
#include <utility>
#include <sstream>

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

std::unique_ptr<ElementNode> Parser::ParseProgram() {
    auto root = std::make_unique<ElementNode>("__ROOT__");
    while (!currentTokenIs(TokenType::Eof)) {
        auto stmt = parseStatement();
        if (stmt) {
            root->children.push_back(std::move(stmt));
        } else {
            if (!currentTokenIs(TokenType::GeneratorComment) && !currentTokenIs(TokenType::Eof)) {
                 m_errors.push_back("Skipping unparseable token: " + m_currentToken.literal);
            }
        }

        if (currentTokenIs(TokenType::Eof)) break;
        nextToken();
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (currentTokenIs(TokenType::GeneratorComment)) {
        return nullptr;
    }

    if (currentTokenIs(TokenType::Identifier)) {
        if (m_currentToken.literal == "text") {
            return parseTextNode();
        }
        return parseElementNode();
    }
    m_errors.push_back("Unexpected token, expected a statement (e.g., an element tag).");
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto element = std::make_unique<ElementNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::LBrace)) return nullptr;

    while (!peekTokenIs(TokenType::RBrace) && !peekTokenIs(TokenType::Eof)) {
        nextToken();

        if (currentTokenIs(TokenType::GeneratorComment)) {
            continue;
        }

        if (!currentTokenIs(TokenType::Identifier)) {
             m_errors.push_back("Unexpected token inside element block.");
             continue;
        }

        if (m_currentToken.literal == "style" && peekTokenIs(TokenType::LBrace)) {
            auto styleNode = parseStyleNode();
            if (styleNode) {
                element->children.push_back(std::move(styleNode));
            }
        } else if (peekTokenIs(TokenType::Colon) || peekTokenIs(TokenType::Equals)) {
            if (m_currentToken.literal == "text") {
                auto textNode = parseTextProperty();
                if (textNode) {
                    element->children.push_back(std::move(textNode));
                }
            } else {
                auto attr = parseAttributeNode();
                if (attr) {
                    element->attributes.push_back(std::move(attr));
                }
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

std::unique_ptr<AttributeNode> Parser::parseAttributeNode() {
    std::string name = m_currentToken.literal;

    if (!peekTokenIs(TokenType::Colon) && !peekTokenIs(TokenType::Equals)) {
        m_errors.push_back("Expected ':' or '=' after attribute name.");
        return nullptr;
    }
    nextToken();

    nextToken();

    if (!currentTokenIs(TokenType::Identifier) && !currentTokenIs(TokenType::String)) {
        m_errors.push_back("Invalid attribute value. Must be an identifier or a string.");
        return nullptr;
    }
    std::string value = m_currentToken.literal;

    auto attr = std::make_unique<AttributeNode>(name, value);

    if (!expectPeek(TokenType::Semicolon)) return nullptr;

    return attr;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    if (!expectPeek(TokenType::LBrace)) return nullptr;

    if (peekTokenIs(TokenType::String)) {
        nextToken();
        auto node = std::make_unique<TextNode>(m_currentToken.literal);
        if (!expectPeek(TokenType::RBrace)) return nullptr;
        return node;
    }

    nextToken();
    std::string content;
    while (!currentTokenIs(TokenType::RBrace) && !currentTokenIs(TokenType::Eof)) {
        content += m_currentToken.literal;
        nextToken();
        if (!currentTokenIs(TokenType::RBrace)) {
             content += " ";
        }
    }

    if (!content.empty() && content.back() == ' ') {
        content.pop_back();
    }

    if (!currentTokenIs(TokenType::RBrace)) {
        m_errors.push_back("Unclosed text block.");
        return nullptr;
    }

    return std::make_unique<TextNode>(content);
}

std::unique_ptr<TextNode> Parser::parseTextProperty() {
    nextToken(); // consume 'text'
    nextToken(); // consume ':' or '='

    std::stringstream content;
    while(!currentTokenIs(TokenType::Semicolon) && !currentTokenIs(TokenType::Eof)) {
        content << m_currentToken.literal;
        if (peekTokenIs(TokenType::Semicolon)) {
            // don't add space before the end
        } else {
            content << " ";
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::Semicolon)) {
        m_errors.push_back("Expected semicolon to end text property.");
        return nullptr;
    }

    return std::make_unique<TextNode>(content.str());
}

std::unique_ptr<StyleNode> Parser::parseStyleNode() {
    if (!expectPeek(TokenType::LBrace)) return nullptr;

    nextToken();

    std::stringstream content;
    int brace_level = 1;

    while (brace_level > 0 && !currentTokenIs(TokenType::Eof)) {
        if (currentTokenIs(TokenType::LBrace)) brace_level++;
        if (currentTokenIs(TokenType::RBrace)) brace_level--;

        if (brace_level > 0) {
            content << m_currentToken.literal << " ";
        }

        // This check ensures we don't advance past the final brace
        if (brace_level > 0) {
            nextToken();
        }
    }

    return std::make_unique<StyleNode>(content.str());
}

} // CHTL
