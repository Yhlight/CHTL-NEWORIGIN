#include "Parser.h"
#include <iostream>
#include <sstream>

Parser::Parser(Lexer& l) : lexer(l) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = lexer.NextToken();
}

void Parser::peekError(TokenType t) {
    std::string error = "Line " + std::to_string(peekToken.line) + ":" + std::to_string(peekToken.column) +
                        " - Expected next token to be " + std::to_string(static_cast<int>(t)) +
                        ", got " + std::to_string(static_cast<int>(peekToken.type)) + " instead.";
    errors.push_back(error);
}

bool Parser::expectPeek(TokenType t) {
    if (peekToken.type == t) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    while (curToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (curToken.type == TokenType::TEXT) {
        return parseTextNode();
    } else if (curToken.type == TokenType::STYLE) {
        return parseStyleNode();
    } else if (curToken.type == TokenType::IDENTIFIER) {
        if (peekToken.type == TokenType::LEFT_BRACE) {
            return parseElementNode();
        } else if (peekToken.type == TokenType::COLON || peekToken.type == TokenType::EQUALS) {
            return parseAttributeNode();
        }
    }
    return nullptr;
}

std::unique_ptr<Statement> Parser::parseStyleNode() {
    Token styleToken = curToken;

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }

    lexer.setSkipWhitespace(false); // Start capturing everything, including whitespace

    std::stringstream content;
    int braceLevel = 1;

    nextToken(); // Consume '{'

    while (braceLevel > 0 && curToken.type != TokenType::END_OF_FILE) {
        if (curToken.type == TokenType::LEFT_BRACE) {
            braceLevel++;
        } else if (curToken.type == TokenType::RIGHT_BRACE) {
            braceLevel--;
        }

        if (braceLevel > 0) {
            content << curToken.literal;
        }
        nextToken();
    }

    lexer.setSkipWhitespace(true); // Restore normal lexer behavior

    if (braceLevel != 0) {
        return nullptr; // Unclosed block
    }

    return std::make_unique<StyleNode>(styleToken, content.str());
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto node = std::make_unique<ElementNode>(curToken, curToken.literal);
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    while (curToken.type != TokenType::RIGHT_BRACE && curToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            node->body->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    if (curToken.type != TokenType::RIGHT_BRACE) return nullptr;
    return node;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    Token textToken = curToken;
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    std::string textValue = curToken.literal;
    if (curToken.type != TokenType::STRING_LITERAL && curToken.type != TokenType::UNQUOTED_LITERAL) {
        return nullptr;
    }
    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;
    return std::make_unique<TextNode>(textToken, textValue);
}

std::unique_ptr<Statement> Parser::parseAttributeNode() {
    auto attrNode = std::make_unique<AttributeNode>(curToken);
    if (peekToken.type != TokenType::COLON && peekToken.type != TokenType::EQUALS) {
        peekError(TokenType::COLON);
        return nullptr;
    }
    nextToken();
    nextToken();
    attrNode->value = parseExpression();
    if (!attrNode->value) return nullptr;
    if (peekToken.type != TokenType::SEMICOLON) {
        peekError(TokenType::SEMICOLON);
        return nullptr;
    }
    nextToken();
    return attrNode;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    if (curToken.type == TokenType::STRING_LITERAL ||
        curToken.type == TokenType::UNQUOTED_LITERAL ||
        curToken.type == TokenType::NUMBER_LITERAL ||
        curToken.type == TokenType::IDENTIFIER) {
        return std::make_unique<LiteralNode>(curToken);
    }
    return nullptr;
}
