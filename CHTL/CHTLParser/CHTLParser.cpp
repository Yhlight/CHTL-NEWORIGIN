#include "CHTLParser.h"
#include <stdexcept>

CHTLParser::CHTLParser(const std::string& input)
    : lexer(input) {
    // Initialize both tokens for lookahead
    advance();
    advance();
}

void CHTLParser::advance() {
    currentToken = nextToken;
    nextToken = lexer.getNextToken();
}

Token CHTLParser::peek() {
    return nextToken;
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
#include "../CHTLNode/StyleNode.h"

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::Identifier) {
        // The 'text' keyword is special; it can start a text block.
        if (currentToken.value == "text" && peek().type == TokenType::LBrace) {
            return parseTextNode();
        }
        if (currentToken.value == "style" && peek().type == TokenType::LBrace) {
            return parseStyleNode();
        }
        return parseElement();
    }
    if (currentToken.type == TokenType::Comment) {
        return parseCommentNode();
    }
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::Identifier); // Consume 'style'
    expect(TokenType::LBrace);

    auto styleNode = std::make_unique<StyleNode>();

    while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a style property name.");
        }
        std::string propName = currentToken.value;
        advance();

        expect(TokenType::Colon);

        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a style property value.");
        }
        std::string propValue = currentToken.value;
        advance();

        expect(TokenType::Semicolon);

        styleNode->addProperty(propName, propValue);
    }

    expect(TokenType::RBrace);
    return styleNode;
}

void CHTLParser::parseAttribute(ElementNode& node) {
    std::string attrName = currentToken.value;
    expect(TokenType::Identifier);

    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) {
        advance(); // Consume ':' or '='
    } else {
        throw std::runtime_error("Expected ':' or '=' after attribute name.");
    }

    if (currentToken.type != TokenType::String && currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected a string literal or an unquoted literal for attribute value.");
    }
    std::string attrValue = currentToken.value;
    advance(); // Consume string or identifier value

    expect(TokenType::Semicolon);

    node.setAttribute(attrName, attrValue);
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

std::unique_ptr<BaseNode> CHTLParser::parseTextAttribute() {
    expect(TokenType::Identifier); // Consume 'text'
    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) {
        advance();
    } else {
        throw std::runtime_error("Expected ':' or '=' after 'text' attribute.");
    }

    if (currentToken.type != TokenType::String) {
        throw std::runtime_error("Expected a string literal for 'text' attribute value.");
    }
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance(); // Consume string value

    expect(TokenType::Semicolon);
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

            if (currentToken.type == TokenType::Identifier && (peek().type == TokenType::Colon || peek().type == TokenType::Assign)) {
                if (currentToken.value == "text") {
                    node->addChild(parseTextAttribute());
                } else {
                    parseAttribute(*node);
                }
            } else {
                auto child = parseStatement();
                if (child) {
                    node->addChild(std::move(child));
                } else {
                    break;
                }
            }
        }
        expect(TokenType::RBrace);
    }

    return node;
}

std::unique_ptr<ElementNode> CHTLParser::parse() {
    auto statement = parseStatement();
    if (!statement) {
        return nullptr;
    }

    // Safely cast the BaseNode to an ElementNode.
    ElementNode* element = dynamic_cast<ElementNode*>(statement.get());
    if (element) {
        // If the cast is successful, release the pointer from the old
        // unique_ptr and transfer ownership to the new one.
        statement.release();
        return std::unique_ptr<ElementNode>(element);
    }

    // If the root is not an element (e.g., a text or comment node),
    // it's a structural error. Throw an exception. The original `statement`
    // unique_ptr will go out of scope and correctly deallocate the memory.
    throw std::runtime_error("The root of a CHTL document must be an element.");
}