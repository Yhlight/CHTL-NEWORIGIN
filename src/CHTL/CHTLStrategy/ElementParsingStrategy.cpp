#include "ElementParsingStrategy.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLState/StatementState.h" // Needed to parse child nodes
#include "../CHTLState/StyleBlockState.h" // Needed for style blocks
#include <sstream>
#include <stdexcept>

std::unique_ptr<BaseNode> ElementParsingStrategy::parse(Parser& parser) {
    auto element = std::make_unique<ElementNode>(parser.currentToken.value);
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    parseElementBody(parser, *element);

    parser.expectToken(TokenType::CloseBrace);
    return element;
}

void ElementParsingStrategy::parseElementBody(Parser& parser, ElementNode& element) {
    // Set the context node so that child parsing can be aware of the parent.
    parser.contextNode = &element;

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style" && parser.peekToken.type == TokenType::OpenBrace) {
            StyleBlockState styleState;
            styleState.handle(parser); // This will parse the entire style block.
        } else if (parser.tryExpectKeyword(TokenType::Except, "KEYWORD_EXCEPT", "except")) {
            // Re-creating the StatementState to access its utility methods.
            // This is a temporary design choice during refactoring.
            StatementState tempState;
            parseExceptClause(parser, element);
        } else if (parser.currentToken.type == TokenType::Identifier && (parser.peekToken.type == TokenType::Colon || parser.peekToken.type == TokenType::Equals)) {
            parseAttribute(parser, element);
        } else {
            // Use a temporary StatementState to parse the child node.
            // This is how we reuse the main dispatcher logic for children.
            StatementState childParserState;
            auto childNode = childParserState.handle(parser);
            if (childNode) {
                 element.children.push_back(std::move(childNode));
            }
        }
    }
    // Reset the context node after parsing the body.
    parser.contextNode = nullptr;
}

void ElementParsingStrategy::parseAttribute(Parser& parser, ElementNode& element) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    parser.advanceTokens(); // Consume ':' or '='

    std::stringstream value;
    bool firstToken = true;
    // Consume tokens until the terminator
    while (parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::EndOfFile) {
        // This check prevents consuming the brace of a parent element if a semicolon is missed.
        if (parser.currentToken.type == TokenType::CloseBrace) {
             throw std::runtime_error("Missing semicolon for attribute '" + key + "'.");
        }
        if (!firstToken) {
            value << " ";
        }
        value << parser.currentToken.value;
        parser.advanceTokens();
        firstToken = false;
    }

    parser.expectToken(TokenType::Semicolon);

    if (key == "text") {
        element.children.push_back(std::make_unique<TextNode>(value.str()));
    } else {
        element.attributes[key] = value.str();
    }
}

void ElementParsingStrategy::parseExceptClause(Parser& parser, ElementNode& element) {
    // The 'except' keyword has already been consumed by tryExpectKeyword in the caller.

    while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::EndOfFile) {
        // For now, we'll just store the raw identifier of the forbidden element.
        // A more advanced implementation would parse complex types like [Custom] @Element Box
        if (parser.currentToken.type == TokenType::Identifier) {
            element.constraints.push_back(parser.currentToken.value);
            parser.advanceTokens();
        }

        // Consume comma if it exists
        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
        } else {
            // If there's no comma, we must be at the end of the list.
            break;
        }
    }
    parser.expectToken(TokenType::Semicolon);
}
