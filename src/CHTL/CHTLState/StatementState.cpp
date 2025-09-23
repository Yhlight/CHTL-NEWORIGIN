#include "StatementState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <stdexcept>
#include <string>
#include <utility> // For std::pair
#include <sstream>

// Forward declare to resolve circular dependency with StyleBlockState.h
class StyleBlockState;
#include "StyleBlockState.h"

// Forward declare to resolve circular dependency between element parsing and statement parsing
class ElementNode;

// The main handler for this state. It acts as a dispatcher.
std::unique_ptr<BaseNode> StatementState::handle(Parser& parser) {
    if (parser.currentToken.type == TokenType::Identifier) {
        if (parser.currentToken.value == "text") {
            return parseTextElement(parser);
        }
        // Any other identifier is assumed to be an element tag.
        return parseElement(parser);
    } else if (parser.currentToken.type == TokenType::HashComment) {
        return parseComment(parser);
    }

    throw std::runtime_error("Statements must begin with an identifier or hash comment. Found '" + parser.currentToken.value + "' instead.");
}

// Parses a full element, including its body.
std::unique_ptr<BaseNode> StatementState::parseElement(Parser& parser) {
    auto element = std::make_unique<ElementNode>(parser.currentToken.value);
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    parseElementBody(parser, *element);

    parser.expectToken(TokenType::CloseBrace);
    return element;
}

// Parses the body of an element, dispatching to helpers for attributes, styles, or child nodes.
void StatementState::parseElementBody(Parser& parser, ElementNode& element) {
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style" && parser.peekToken.type == TokenType::OpenBrace) {
            // Set context for the style state and use it to parse the block.
            parser.contextNode = &element;
            StyleBlockState styleState;
            styleState.handle(parser); // This will parse the entire style block.
            parser.contextNode = nullptr; // Reset context.
        } else if (parser.currentToken.type == TokenType::Identifier && (parser.peekToken.type == TokenType::Colon || parser.peekToken.type == TokenType::Equals)) {
            parseAttribute(parser, element);
        } else {
            element.children.push_back(handle(parser));
        }
    }
}

// Parses a 'text { "..." }' block.
std::unique_ptr<BaseNode> StatementState::parseTextElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // consume 'text'
    parser.expectToken(TokenType::OpenBrace);
    std::string textContent = parser.currentToken.value;
    parser.expectToken(TokenType::String);
    parser.expectToken(TokenType::CloseBrace);
    return std::make_unique<TextNode>(textContent);
}

// Parses a '# comment' line.
std::unique_ptr<BaseNode> StatementState::parseComment(Parser& parser) {
    auto comment = std::make_unique<CommentNode>(parser.currentToken.value);
    parser.expectToken(TokenType::HashComment);
    return comment;
}

// Parses an attribute 'key: value;'.
void StatementState::parseAttribute(Parser& parser, ElementNode& element) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    parser.advanceTokens();

    std::string value;
    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        value = parser.currentToken.value;
        parser.advanceTokens();
    } else {
        throw std::runtime_error("Attribute value for '" + key + "' must be an identifier or a string.");
    }

    parser.expectToken(TokenType::Semicolon);

    if (key == "text") {
        element.children.push_back(std::make_unique<TextNode>(value));
    } else {
        element.attributes[key] = value;
    }
}
