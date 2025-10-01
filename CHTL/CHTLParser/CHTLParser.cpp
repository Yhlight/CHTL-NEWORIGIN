#include "CHTLParser.h"
#include <stdexcept>

CHTLParser::CHTLParser(const std::string& input)
    : lexer(input), currentToken(lexer.getNextToken()) {}

void CHTLParser::advance() {
    currentToken = lexer.getNextToken();
}

void CHTLParser::expect(TokenType type) {
    if (currentToken.type == type) {
        advance();
    } else {
        throw std::runtime_error("Unexpected token");
    }
}

#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text") {
            return parseTextNode();
        }
        return parseElement();
    }
    if (currentToken.type == TokenType::Comment) {
        return parseCommentNode();
    }
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextNode() {
    expect(TokenType::Identifier); // Consume 'text'
    expect(TokenType::LBrace);
    if (currentToken.type != TokenType::String) {
         throw std::runtime_error("Expected a string literal inside text block.");
    }
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance(); // Consume string
    expect(TokenType::RBrace);
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseCommentNode() {
    auto node = std::make_unique<CommentNode>(currentToken.value);
    advance(); // Consume comment token
    return node;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    if (currentToken.type != TokenType::Identifier) {
        return nullptr;
    }

    auto node = std::make_unique<ElementNode>(currentToken.value);
    advance(); // Consume identifier

    if (currentToken.type == TokenType::LBrace) {
        advance(); // Consume '{'
        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            auto child = parseStatement();
            if (child) {
                node->addChild(std::move(child));
            } else {
                // If parseStatement returns nullptr, it means we've hit a token
                // that doesn't start a valid statement, so we should break.
                break;
            }
        }
        expect(TokenType::RBrace);
    }

    return node;
}

std::unique_ptr<ElementNode> CHTLParser::parse() {
    // The root must be an element node.
    auto statement = parseStatement();
    // This is a dynamic_cast which is generally not ideal, but for the root of the tree it's acceptable.
    // A better approach might be to have a DocumentNode that can hold various top-level nodes.
    ElementNode* raw_ptr = dynamic_cast<ElementNode*>(statement.release());
    return std::unique_ptr<ElementNode>(raw_ptr);
}