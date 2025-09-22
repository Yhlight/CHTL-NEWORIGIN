#include "Parser.h"
#include <memory>

namespace CHTL {

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    // Prime the tokens by calling nextToken twice to initialize currentToken and peekToken
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();

    // The parser will ignore comments, so we just keep advancing
    // until we get a non-comment token.
    while (peekToken.type == TokenType::LINE_COMMENT || peekToken.type == TokenType::BLOCK_COMMENT) {
        peekToken = lexer.nextToken();
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    program->token = currentToken;

    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program->children.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        if (currentToken.literal == "text") {
            return parseTextNode();
        } else {
            // Assume any other identifier at the start of a statement is an element tag
            return parseElementNode();
        }
    }
    // Return null if the token doesn't start a known statement type
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto elementNode = std::make_unique<ElementNode>();
    elementNode->token = currentToken;
    elementNode->tagName = currentToken.literal;

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr; // Error: expected '{'
    }

    nextToken(); // Consume '{'

    // Parse child statements
    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            elementNode->children.push_back(std::move(stmt));
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RBRACE)) {
        // Error: unclosed element. The error is already logged by expectPeek if it was called.
        // If we just run out of tokens, we might need another error here.
        return nullptr;
    }

    return elementNode;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto textNode = std::make_unique<TextNode>();
    textNode->token = currentToken; // The 'text' token

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr; // Error: expected '{'
    }

    nextToken(); // Consume '{'

    // CHTL allows unquoted literals, which our lexer tokenizes as IDENTIFIER.
    if (currentTokenIs(TokenType::STRING_LITERAL) || currentTokenIs(TokenType::IDENTIFIER)) {
        textNode->value = currentToken.literal;
    } else {
        // Error: expected a literal inside text block
        return nullptr;
    }

    if (!expectPeek(TokenType::RBRACE)) {
        return nullptr; // Error: unclosed text block
    }

    return textNode;
}

// Helper methods
bool Parser::currentTokenIs(TokenType type) const {
    return currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) const {
    return peekToken.type == type;
}

bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    } else {
        peekError(type);
        return false;
    }
}

void Parser::peekError(TokenType type) {
    std::string error = "Error: Expected next token to be " + std::to_string(static_cast<int>(type)) +
                        ", got " + std::to_string(static_cast<int>(peekToken.type)) +
                        " instead. Line: " + std::to_string(peekToken.line);
    errors.push_back(error);
}

} // namespace CHTL
