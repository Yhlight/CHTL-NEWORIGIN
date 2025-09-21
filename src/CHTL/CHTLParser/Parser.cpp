#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    // Prime the parser with the first two tokens
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::currentTokenIs(TokenType type) const {
    return currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) const {
    return peekToken.type == type;
}

void Parser::peekError(TokenType type) {
    std::string error = "Line " + std::to_string(peekToken.line) + ":" + std::to_string(peekToken.column) +
                        " - Expected next token to be " + TokenTypeStrings.at(type) +
                        ", got " + TokenTypeStrings.at(peekToken.type) + " instead.";
    errors.push_back(error);
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

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();

    while (!currentTokenIs(END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program->children.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Node> Parser::parseStatement() {
    if (currentTokenIs(IDENTIFIER)) {
        if (currentToken.literal == "text") {
            return parseText();
        }
        return parseElement();
    }
    return nullptr;
}


// An attribute is not a node, so it doesn't return a Node pointer.
// It's parsed inside a block and added directly to the parent element.
std::optional<Attribute> Parser::parseAttribute() {
    Attribute attr;
    if (!currentTokenIs(IDENTIFIER)) return std::nullopt;
    attr.name = currentToken.literal;

    // Expect ':' or '='
    if (!peekTokenIs(COLON) && !peekTokenIs(EQUALS)) {
        return std::nullopt;
    }
    nextToken(); // consume identifier

    // currentToken is now ':' or '='
    nextToken(); // consume ':' or '='

    if (!currentTokenIs(STRING)) {
        // For now we only support string literals for values.
        // Later we will support unquoted literals.
        errors.push_back("Expected string literal for attribute value.");
        return std::nullopt;
    }
    attr.value = currentToken.literal;

    if (!expectPeek(SEMICOLON)) {
        return std::nullopt; // expectPeek handles the error message
    }

    return attr;
}


std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = currentToken.literal;

    if (!expectPeek(LBRACE)) {
        return nullptr;
    }

    parseBlock(element.get());

    return element;
}

void Parser::parseBlock(ElementNode* element) {
    nextToken(); // Consume '{'

    while (!currentTokenIs(RBRACE) && !currentTokenIs(END_OF_FILE)) {
        // Check if the statement is an attribute
        if (currentTokenIs(IDENTIFIER) && (peekTokenIs(COLON) || peekTokenIs(EQUALS))) {
            if (auto attr = parseAttribute()) {
                element->attributes.push_back(attr.value());
            }
        } else { // Otherwise, it's a child node
            auto stmt = parseStatement();
            if (stmt) {
                element->children.push_back(std::move(stmt));
            }
        }
        nextToken(); // Move to the next statement/attribute
    }
}

std::unique_ptr<TextNode> Parser::parseText() {
    // currentToken is 'text'
    if (!expectPeek(LBRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    if (!currentTokenIs(STRING)) {
        // Error: expected a string literal inside text block
        return nullptr;
    }

    auto textNode = std::make_unique<TextNode>();
    textNode->value = currentToken.literal;

    if (!expectPeek(RBRACE)) {
        return nullptr;
    }

    return textNode;
}

} // namespace CHTL
