#include "Parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}

std::unique_ptr<RootNode> Parser::parse() {
    NodeList statements;
    while (currentToken().type != TokenType::EndOfFile) {
        NodePtr stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    return std::make_unique<RootNode>(std::move(statements));
}

NodePtr Parser::parseStatement() {
    if (currentToken().type == TokenType::Template) {
        return parseTemplateDefinition();
    }
    if (currentToken().type == TokenType::Identifier && peekToken().type == TokenType::OpenBrace) {
        return parseElement();
    }
    if (currentToken().type == TokenType::Text) {
        return parseText();
    }
    std::cerr << "Parser Error: Unexpected top-level token " << tokenTypeToString(currentToken().type) << " at line " << currentToken().line << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    Token elementToken = currentToken();
    expect(TokenType::Identifier, "Expected element name.");
    auto element = std::make_unique<ElementNode>(elementToken.value);
    expect(TokenType::OpenBrace, "Expected '{' after element name.");

    while (currentToken().type != TokenType::CloseBrace && currentToken().type != TokenType::EndOfFile) {
        if (currentToken().type == TokenType::Identifier && (peekToken().type == TokenType::Colon || peekToken().type == TokenType::Equals)) {
            Token key = currentToken();
            advance();
            advance();
            Token value = currentToken();
            if (value.type != TokenType::String && value.type != TokenType::Identifier) {
                std::cerr << "Parser Error: Unexpected token for attribute value." << std::endl;
                break;
            }
            advance();
            element->addAttribute(std::make_unique<AttributeNode>(key.value, value.value));
            if (currentToken().type == TokenType::Semicolon) advance();
        } else if (currentToken().type == TokenType::Style) {
            parseStyleBlock(element.get());
        } else if (currentToken().type == TokenType::String) {
            Token textToken = currentToken();
            advance();
            element->addChild(std::make_unique<TextNode>(textToken.value));
        } else {
            NodePtr child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            } else {
                std::cerr << "Parser Error: Unexpected token " << tokenTypeToString(currentToken().type) << " in element body." << std::endl;
                advance();
            }
        }
    }
    expect(TokenType::CloseBrace, "Expected '}' to close element.");
    return element;
}

void Parser::parseStyleBlock(ElementNode* element) {
    expect(TokenType::Style, "Expected 'style' keyword.");
    expect(TokenType::OpenBrace, "Expected '{' after 'style'.");

    while (currentToken().type != TokenType::CloseBrace && currentToken().type != TokenType::EndOfFile) {
        if (currentToken().type == TokenType::AtStyle) {
            advance(); // Consume @Style
            Token name = currentToken();
            expect(TokenType::Identifier, "Expected template name after @Style.");
            element->addStyleStatement(std::make_unique<TemplateUsageNode>(TemplateType::Style, name.value));
            expect(TokenType::Semicolon, "Expected ';' after style template usage.");
        } else {
            std::string property_str;
            while (currentToken().type != TokenType::Colon && currentToken().type != TokenType::EndOfFile) {
                property_str += currentToken().value;
                advance();
            }
            expect(TokenType::Colon, "Expected ':' after style property name.");
            std::string value_str;
            while (currentToken().type != TokenType::Semicolon && currentToken().type != TokenType::CloseBrace) {
                value_str += currentToken().value;
                advance();
            }
            element->addStyleStatement(std::make_unique<StylePropertyNode>(property_str, value_str));
            if (currentToken().type == TokenType::Semicolon) advance();
        }
    }
    expect(TokenType::CloseBrace, "Expected '}' to close style block.");
}

NodePtr Parser::parseTemplateDefinition() {
    expect(TokenType::Template, "Expected [Template].");

    TemplateType type;
    if (match(TokenType::AtStyle)) type = TemplateType::Style;
    else if (match(TokenType::AtElement)) type = TemplateType::Element;
    else if (match(TokenType::AtVar)) type = TemplateType::Var;
    else {
        std::cerr << "Parser Error: Expected @Style, @Element, or @Var after [Template]." << std::endl;
        return nullptr;
    }

    Token name = currentToken();
    expect(TokenType::Identifier, "Expected template name.");
    expect(TokenType::OpenBrace, "Expected '{' for template body.");

    std::vector<Token> bodyTokens;
    int braceDepth = 1;
    while (braceDepth > 0 && currentToken().type != TokenType::EndOfFile) {
        if (currentToken().type == TokenType::OpenBrace) braceDepth++;
        if (currentToken().type == TokenType::CloseBrace) braceDepth--;

        if (braceDepth > 0) {
            bodyTokens.push_back(currentToken());
            advance();
        }
    }

    expect(TokenType::CloseBrace, "Expected '}' to close template body.");

    auto body = std::make_unique<RawBodyNode>(std::move(bodyTokens));
    return std::make_unique<TemplateDefinitionNode>(type, name.value, std::move(body));
}


NodePtr Parser::parseText() {
    expect(TokenType::Text, "Expected 'text' keyword.");
    expect(TokenType::OpenBrace, "Expected '{' after 'text'.");
    Token content = currentToken();
    expect(TokenType::String, "Expected a string literal for text content.");
    auto textNode = std::make_unique<TextNode>(content.value);
    expect(TokenType::CloseBrace, "Expected '}' to close text block.");
    return textNode;
}

Token Parser::currentToken() {
    if (position >= tokens.size()) return {TokenType::EndOfFile, "", 0, 0};
    return tokens[position];
}

Token Parser::peekToken() {
    if (position + 1 >= tokens.size()) return {TokenType::EndOfFile, "", 0, 0};
    return tokens[position + 1];
}

void Parser::advance() {
    if (position < tokens.size()) position++;
}

bool Parser::match(TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& errorMessage) {
    if (currentToken().type == type) {
        advance();
    } else {
        std::cerr << "Parser Error: " << errorMessage << " Got " << tokenTypeToString(currentToken().type)
                  << " instead. (Line: " << currentToken().line << ", Col: " << currentToken().column << ")" << std::endl;
        exit(1);
    }
}
