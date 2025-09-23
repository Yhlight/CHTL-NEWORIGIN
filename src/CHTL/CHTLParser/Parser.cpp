#include "Parser.h"
#include "../CHTLNode/CommentNode.h"
#include <stdexcept>
#include <string>

Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken({TokenType::Unexpected, ""}), peekToken({TokenType::Unexpected, ""}) {
    // Initialize the token stream by calling advanceTokens twice.
    advanceTokens();
    advanceTokens();
}

void Parser::advanceTokens() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

void Parser::expectToken(TokenType type) {
    if (currentToken.type == type) {
        advanceTokens();
    } else {
        // Basic error handling for now. This could be improved with more detailed error messages.
        throw std::runtime_error("Unexpected token: '" + currentToken.value + "' at line " +
                                 std::to_string(currentToken.line) + ", column " +
                                 std::to_string(currentToken.column) + ". Expected type " +
                                 std::to_string(static_cast<int>(type)));
    }
}

std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> statements;
    while (currentToken.type != TokenType::EndOfFile) {
        statements.push_back(parseStatement());
    }
    return statements;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    // The type of statement is determined by the current token.
    if (currentToken.type == TokenType::Identifier) {
        // Check if it's a 'text' element or a regular element.
        if (currentToken.value == "text") {
            return parseTextElement();
        }
        return parseElement();
    } else if (currentToken.type == TokenType::HashComment) {
        return parseComment();
    }

    // For now, only identifiers or comments can start a statement.
    throw std::runtime_error("Statements must begin with an identifier or hash comment. Found '" + currentToken.value + "' instead.");
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>(currentToken.value);
    expectToken(TokenType::Identifier);

    expectToken(TokenType::OpenBrace);

    // Inside an element, we can have attributes or child nodes.
    while (currentToken.type != TokenType::CloseBrace && currentToken.type != TokenType::EndOfFile) {
        // Check for a style block first, e.g., 'style { ... }'
        if (currentToken.type == TokenType::Identifier && currentToken.value == "style" && peekToken.type == TokenType::OpenBrace) {
            parseStyleBlock(element.get());
        } else if (currentToken.type == TokenType::Identifier && (peekToken.type == TokenType::Colon || peekToken.type == TokenType::Equals)) {
            // If not a style block, check if it's an attribute, e.g., 'id: my-id;'
            parseAttribute(element.get());
        } else {
            // Otherwise, it must be a child node (element, text block, comment, etc.).
            element->children.push_back(parseStatement());
        }
    }

    expectToken(TokenType::CloseBrace);

    return element;
}

std::unique_ptr<TextNode> Parser::parseTextElement() {
    // Consume the 'text' identifier.
    expectToken(TokenType::Identifier);

    expectToken(TokenType::OpenBrace);

    // The content of the text block must be a single string literal.
    std::string textContent = currentToken.value;
    expectToken(TokenType::String);

    expectToken(TokenType::CloseBrace);

    return std::make_unique<TextNode>(textContent);
}

std::unique_ptr<BaseNode> Parser::parseComment() {
    auto comment = std::make_unique<CommentNode>(currentToken.value);
    expectToken(TokenType::HashComment);
    return comment;
}

void Parser::parseAttribute(ElementNode* element) {
    std::string key = currentToken.value;
    expectToken(TokenType::Identifier);

    // An attribute can be defined with either ':' or '='.
    if (currentToken.type != TokenType::Colon && currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    advanceTokens(); // Consume ':' or '='

    // The attribute value can be an unquoted literal (tokenized as Identifier) or a string.
    std::string value;
    if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::String) {
        value = currentToken.value;
        advanceTokens();
    } else {
        throw std::runtime_error("Attribute value for '" + key + "' must be an identifier or a string.");
    }

    expectToken(TokenType::Semicolon);

    // The 'text' attribute is special; it creates a child TextNode instead of a map entry.
    if (key == "text") {
        element->children.push_back(std::make_unique<TextNode>(value));
    } else {
        element->attributes[key] = value;
    }
}

void Parser::parseStyleBlock(ElementNode* element) {
    expectToken(TokenType::Identifier); // consume 'style'
    expectToken(TokenType::OpenBrace);

    std::string styleString;

    // Loop through all the properties inside the style block.
    while (currentToken.type != TokenType::CloseBrace && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a style property name (identifier) at line " + std::to_string(currentToken.line));
        }
        std::string key = currentToken.value;
        advanceTokens();

        expectToken(TokenType::Colon);

        // The value can be a sequence of tokens until the semicolon.
        std::string value;
        while (currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::CloseBrace) {
            value += currentToken.value;
            advanceTokens();
            // Add a space between parts of a multi-part value (e.g., "1px solid black").
            if (currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::CloseBrace) {
                value += " ";
            }
        }

        // Remove trailing space if it exists.
        if (!value.empty() && value.back() == ' ') {
            value.pop_back();
        }

        // Semicolon is optional for the last property before the '}'.
        if (currentToken.type == TokenType::Semicolon) {
            advanceTokens();
        }

        styleString += key + ": " + value + "; ";
    }

    expectToken(TokenType::CloseBrace);

    // Append the parsed styles to the element's style attribute.
    if (element->attributes.count("style")) {
        element->attributes["style"] += styleString;
    } else {
        element->attributes["style"] = styleString;
    }
}
