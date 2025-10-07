#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    consume();
    consume(); // Initialize both currentToken and peekToken
}

void Parser::consume() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        consume();
    } else {
        std::string err = "Unexpected token: expected " + std::to_string((int)type) +
                          ", got " + std::to_string((int)currentToken.type) +
                          " ('" + currentToken.value + "') at line " + std::to_string(currentToken.line) +
                          ", col " + std::to_string(currentToken.column);
        throw std::runtime_error(err);
    }
}

NodeList Parser::parse() {
    NodeList nodes;
    while(currentToken.type != TokenType::EndOfFile) {
        // Skip comments that are not part of the AST
        while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) {
            consume();
        }
        if (currentToken.type == TokenType::EndOfFile) break;

        if (currentToken.type == TokenType::LeftBracket) {
            parseTemplateDefinition();
        } else {
            NodePtr stmt = parseStatement();
            if (stmt) {
                nodes.push_back(stmt);
            } else {
                throw std::runtime_error("Unexpected top-level token: " + currentToken.value);
            }
        }
    }
    return nodes;
}

NodePtr Parser::parseStatement() {
    if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text") {
            return parseText();
        }
        return parseElement();
    } else if (currentToken.type == TokenType::GeneratorComment) {
        return parseComment();
    }
    return nullptr;
}

NodePtr Parser::parseElement() {
    std::string tagName = currentToken.value;
    eat(TokenType::Identifier);

    auto element = std::make_shared<ElementNode>(tagName);

    eat(TokenType::LeftBrace);

    while (currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        // Skip non-generator comments
        while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) {
            consume();
        }
        if (currentToken.type == TokenType::RightBrace || currentToken.type == TokenType::EndOfFile) break;

        if (currentToken.type == TokenType::Identifier && (peekToken.type == TokenType::Colon || peekToken.type == TokenType::Equal)) {
            parseAttributes(element);
        } else if (currentToken.type == TokenType::Identifier && currentToken.value == "style" && peekToken.type == TokenType::LeftBrace) {
            parseStyleBlock(element);
        } else {
            NodePtr child = parseStatement();
            if (child) {
                element->children.push_back(child);
            } else {
                 throw std::runtime_error("Unexpected token in element body: " + currentToken.value);
            }
        }
    }

    eat(TokenType::RightBrace);

    return element;
}

void Parser::parseAttributes(std::shared_ptr<ElementNode> element) {
    std::string key = currentToken.value;
    eat(TokenType::Identifier);

    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Equal) {
        eat(currentToken.type);
    } else {
        throw std::runtime_error("Expected ':' or '=' for attribute.");
    }

    std::string value;
    if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::StringLiteral) {
        value = currentToken.value;
        eat(currentToken.type);
    } else {
        throw std::runtime_error("Attribute value must be an identifier or a string literal.");
    }

    element->attributes[key] = value;
    eat(TokenType::Semicolon);
}

NodePtr Parser::parseText() {
    eat(TokenType::Identifier); // consume 'text'
    eat(TokenType::LeftBrace);
    std::string content = currentToken.value;
    eat(TokenType::StringLiteral);
    eat(TokenType::RightBrace);

    return std::make_shared<TextNode>(content);
}

NodePtr Parser::parseComment() {
    std::string content = currentToken.value;
    eat(TokenType::GeneratorComment);
    return std::make_shared<CommentNode>(content);
}

void Parser::parseStyleBlock(std::shared_ptr<ElementNode> element) {
    eat(TokenType::Identifier); // consume 'style'
    eat(TokenType::LeftBrace);

    std::stringstream style_content;
    while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::At) {
            eat(TokenType::At);
            if (currentToken.value != "Style") {
                throw std::runtime_error("Expected 'Style' keyword for template usage.");
            }
            eat(TokenType::Identifier); // Consume "Style"

            std::string templateName = currentToken.value;
            eat(TokenType::Identifier);

            style_content << TemplateRegistry::getStyleTemplate(templateName);

            eat(TokenType::Semicolon);
        } else {
            std::string property = currentToken.value;
            eat(TokenType::Identifier);

            eat(TokenType::Colon);

            std::string value = currentToken.value;
            eat(TokenType::Identifier);

            style_content << property << ":" << value << ";";

            eat(TokenType::Semicolon);
        }
    }

    eat(TokenType::RightBrace);

    // Add or append to the style attribute
    if (element->attributes.count("style")) {
        element->attributes["style"] += style_content.str();
    } else {
        element->attributes["style"] = style_content.str();
    }
}

NodePtr Parser::parseTemplateDefinition() {
    eat(TokenType::LeftBracket);
    if (currentToken.value != "Template") {
        throw std::runtime_error("Expected 'Template' keyword in template definition.");
    }
    eat(TokenType::Identifier);
    eat(TokenType::RightBracket);

    eat(TokenType::At);
    if (currentToken.value != "Style") {
        throw std::runtime_error("Expected 'Style' keyword in template definition.");
    }
    eat(TokenType::Identifier);

    std::string templateName = currentToken.value;
    eat(TokenType::Identifier);

    eat(TokenType::LeftBrace);

    std::stringstream template_content;
    while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        std::string property = currentToken.value;
        eat(TokenType::Identifier);
        eat(TokenType::Colon);
        std::string value = currentToken.value;
        eat(TokenType::Identifier);
        eat(TokenType::Semicolon);
        template_content << property << ":" << value << ";";
    }

    eat(TokenType::RightBrace);

    TemplateRegistry::registerStyleTemplate(templateName, template_content.str());

    return nullptr; // Definitions don't produce a node in the main AST
}