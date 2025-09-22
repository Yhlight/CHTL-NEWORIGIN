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
        } else if (currentToken.literal == "style") {
            return parseStyleNode();
        }
        else {
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

    // Parse child statements and attributes
    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        // Check for an attribute: IDENTIFIER followed by : or =
        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            Attribute attr;
            attr.token = currentToken;
            attr.key = currentToken.literal;

            nextToken(); // consume identifier, currentToken is now : or =
            nextToken(); // consume : or =, currentToken is now the value

            if (currentTokenIs(TokenType::STRING_LITERAL) || currentTokenIs(TokenType::IDENTIFIER)) {
                attr.value = currentToken.literal;
            } else {
                // Error: invalid attribute value
                // For now, we can just stop parsing this element
                break;
            }

            elementNode->attributes.push_back(attr);

            // Attributes must end with a semicolon
            if (!expectPeek(TokenType::SEMICOLON)) {
                // Error is reported by expectPeek
                return nullptr;
            }
        } else {
            // If it's not an attribute, it must be a child statement
            auto stmt = parseStatement();
            if (stmt) {
                elementNode->children.push_back(std::move(stmt));
            }
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
    std::string error = "Error on line " + std::to_string(peekToken.line) +
                        ": Expected next token to be type " + std::to_string(static_cast<int>(type)) +
                        ", but got type " + std::to_string(static_cast<int>(peekToken.type)) +
                        " instead. (Current token: '" + currentToken.literal + "')";
    errors.push_back(error);
}

std::unique_ptr<StyleNode> Parser::parseStyleNode() {
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->token = currentToken;

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }

    nextToken(); // consume '{'

    // Loop until we find the closing brace
    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        // We expect a CSS property, which is an identifier
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            StyleProperty prop;
            prop.token = currentToken;
            prop.key = currentToken.literal;

            if (!expectPeek(TokenType::COLON)) {
                return nullptr;
            }
            nextToken(); // consume ':'

            // Read all tokens until the semicolon as the value.
            // This is a simple way to handle multi-token values like `100px`.
            std::string value_str;
            // Loop while the *next* token is not the semicolon that terminates the property.
            while (!peekTokenIs(TokenType::SEMICOLON) && !peekTokenIs(TokenType::RBRACE) && !peekTokenIs(TokenType::END_OF_FILE)) {
                value_str += currentToken.literal;
                nextToken();
            }
            // Add the final token of the value (the one before the semicolon)
            value_str += currentToken.literal;
            prop.value = value_str;

            styleNode->properties.push_back(prop);

            // Properties must end with a semicolon
            if (!expectPeek(TokenType::SEMICOLON)) {
                return nullptr;
            }
        }

        nextToken(); // Advance to the next property or the closing brace
    }

    return styleNode;
}

} // namespace CHTL
