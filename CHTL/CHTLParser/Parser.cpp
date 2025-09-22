#include "Parser.h"
#include <iostream>

CHTLParser::CHTLParser(std::vector<Token> tokens) : tokens(std::move(tokens)) {
    states.push_back(ParserState::GLOBAL);
}

CHTLParser::ParserState CHTLParser::currentState() {
    if (states.empty()) return ParserState::GLOBAL; // Should not happen
    return states.back();
}

std::vector<std::shared_ptr<BaseNode>> CHTLParser::parse() {
    std::vector<std::shared_ptr<BaseNode>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(parseStatement());
        } catch (const ParseError& error) {
            std::cerr << "Parse Error: " << error.what() << std::endl;
            synchronize();
        }
    }
    return statements;
}

std::shared_ptr<BaseNode> CHTLParser::parseStatement() {
    if (peek().type == TokenType::GENERATOR_COMMENT) {
        Token commentToken = advance();
        return std::make_shared<CommentNode>(commentToken.lexeme);
    }

    if (peek().type == TokenType::LEFT_BRACKET) {
        parseTemplateDefinition();
        return nullptr; // Template definitions don't produce a node in the main AST
    }

    if (peek().type == TokenType::IDENTIFIER) {
        // Could be a text element or a regular element.
        if (peek().lexeme == "text") {
            return parseTextElement();
        }
        return parseElement();
    }

    // If we don't recognize the statement, advance and try again.
    // This is basic error handling.
    advance();
    return nullptr;
}

void CHTLParser::parseAttribute(std::shared_ptr<ElementNode> element) {
    Token key = consume(TokenType::IDENTIFIER, "Expected attribute name.");

    if (!match({TokenType::COLON, TokenType::EQUALS})) {
        throw ParseError("Expected ':' or '=' after attribute name.");
    }

    if (match({TokenType::STRING_LITERAL, TokenType::SINGLE_QUOTED_LITERAL, TokenType::IDENTIFIER, TokenType::UNQUOTED_LITERAL})) {
        element->attributes[key.lexeme] = previous().lexeme;
    } else {
        throw ParseError("Expected attribute value.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
}

void CHTLParser::parseStyleBlock(std::shared_ptr<ElementNode> element) {
    consume(TokenType::IDENTIFIER, "Expected 'style' keyword."); // consume 'style'
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style' keyword.");

    states.push_back(ParserState::STYLE_BLOCK);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::AT)) {
            // It's a template usage
            consume(TokenType::AT, "Expected '@'.");
            Token type = consume(TokenType::IDENTIFIER, "Expected template type, e.g., 'Style'.");
            if (type.lexeme == "Style") {
                Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
                auto styleTemplate = parseContext.findStyleTemplate(name.lexeme);
                if (styleTemplate) {
                    for (const auto& prop : styleTemplate->properties) {
                        element->inlineStyles[prop.first] = prop.second;
                    }
                } else {
                    throw ParseError("Undefined style template: " + name.lexeme);
                }
                consume(TokenType::SEMICOLON, "Expected ';' after template usage.");
            }
        } else if (check(TokenType::DOT) || check(TokenType::HASH)) {
            // It's a selector block for global styles
            std::string selector;
            if (match({TokenType::DOT})) {
                selector = "." + consume(TokenType::IDENTIFIER, "Expected class name after '.'.").lexeme;
                element->attributes["class"] = selector.substr(1);
            } else if (match({TokenType::HASH})) {
                selector = "#" + consume(TokenType::IDENTIFIER, "Expected id name after '#'.").lexeme;
                element->attributes["id"] = selector.substr(1);
            }

            CssRule rule;
            rule.selector = selector;

            consume(TokenType::LEFT_BRACE, "Expected '{' after selector.");
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
                consume(TokenType::COLON, "Expected ':' after style property name.");
                std::string value = parseValueUntilSemicolon();
                rule.properties[key.lexeme] = value;
            }
            consume(TokenType::RIGHT_BRACE, "Expected '}' after selector block.");
            element->globalCssRules.push_back(rule);

        } else {
            // It's an inline style property
            Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
            consume(TokenType::COLON, "Expected ':' after style property name.");
            std::string value = parseValueUntilSemicolon();
            element->inlineStyles[key.lexeme] = value;
        }
    }

    states.pop_back();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block.");
}

std::string CHTLParser::parseValueUntilSemicolon() {
    std::string value = "";
    while(!check(TokenType::SEMICOLON) && !isAtEnd()) {
        value += advance().lexeme;
        if(peek().type != TokenType::SEMICOLON) {
            value += " ";
        }
    }
    if (!value.empty() && value.back() == ' ') {
        value.pop_back();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after value.");
    return value;
}

void CHTLParser::parseTemplateDefinition() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' to start template definition.");
    consume(TokenType::IDENTIFIER, "Expected 'Template' keyword."); // Assuming "Template" is lexed as IDENTIFIER
    consume(TokenType::RIGHT_BRACKET, "Expected ']' to end template keyword.");

    consume(TokenType::AT, "Expected '@' for template type.");
    Token type = consume(TokenType::IDENTIFIER, "Expected template type (e.g., 'Style').");

    if (type.lexeme == "Style") {
        Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
        consume(TokenType::LEFT_BRACE, "Expected '{' for template body.");

        StyleTemplate styleTemplate;
        styleTemplate.name = name.lexeme;

        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
            consume(TokenType::COLON, "Expected ':' after style property name.");
            std::string value = parseValueUntilSemicolon();
            styleTemplate.properties[key.lexeme] = value;
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");
        parseContext.addStyleTemplate(name.lexeme, std::move(styleTemplate));
    }
    // else if type is Element, Var, etc.
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expected element name.");
    auto element = std::make_shared<ElementNode>(identifier.lexeme);

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name.");

    states.push_back(ParserState::ELEMENT_BODY);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // In an element block, we can have attributes or child nodes.
        // We can distinguish them by looking ahead. An attribute starts with
        // Check for an attribute
        if (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS) {
            parseAttribute(element);
        }
        // Check for a style block
        else if (peek().lexeme == "style") {
            parseStyleBlock(element);
        }
        // Otherwise, it must be a child node
        else {
            auto child = parseStatement();
            if (child) {
                element->children.push_back(child);
            }
        }
    }

    states.pop_back();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after element block.");
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseTextElement() {
    consume(TokenType::IDENTIFIER, "Expected 'text' keyword."); // Consume 'text'
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text' keyword.");

    // For now, we'll assume the text is a single string literal.
    // CHTL also supports unquoted literals here.
    Token content = consume(TokenType::STRING_LITERAL, "Expected string literal inside text block.");

    consume(TokenType::RIGHT_BRACE, "Expected '}' after text block.");

    return std::make_shared<TextNode>(content.lexeme);
}


// --- Helper Methods ---

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token& CHTLParser::peek() {
    return tokens[current];
}

Token& CHTLParser::previous() {
    return tokens[current - 1];
}

Token& CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token& CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw ParseError("Line " + std::to_string(peek().line) + ": " + message);
}

void CHTLParser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            // Should probably add keywords here later
            case TokenType::LEFT_BRACE:
            case TokenType::RIGHT_BRACE:
                return;
            default:
                break;
        }
        advance();
    }
}
