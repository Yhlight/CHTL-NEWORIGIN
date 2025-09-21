#include "Parser.h"
#include <iostream>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<NodePtr> Parser::parse() {
    std::vector<NodePtr> nodes;
    while(!isAtEnd()) {
        nodes.push_back(parseDeclaration());
    }
    return nodes;
}

NodePtr Parser::parseDeclaration() {
    if (match({TokenType::KEYWORD_IMPORT})) {
        return parseImport();
    }
    // other declarations like [Template] go here

    return parseStatement();
}

NodePtr Parser::parseStatement() {
    if(check(TokenType::IDENTIFIER)) {
        return parseElement();
    }
    // for now, skip unknown tokens at top level
    advance();
    return nullptr;
}


NodePtr Parser::parseImport() {
    // Already consumed [Import]
    consume(TokenType::AT_CHTL, "Only @Chtl imports are supported for now.");
    consume(TokenType::KEYWORD_FROM, "Expect 'from' after import type.");

    Token pathToken;
    if (match({TokenType::STRING, TokenType::UNQUOTED_LITERAL})) {
        pathToken = previous();
    } else {
        // error
        consume(TokenType::STRING, "Expect a path string literal.");
    }

    auto importNode = std::make_shared<ImportNode>();
    importNode->type = ImportNode::ImportType::CHTL;
    importNode->path = pathToken.lexeme;

    if (match({TokenType::KEYWORD_AS})) {
        Token aliasToken = consume(TokenType::IDENTIFIER, "Expect alias after 'as'.");
        importNode->alias = aliasToken.lexeme;
    }

    return importNode;
}


NodePtr Parser::parseElement() {
    if (check(TokenType::IDENTIFIER)) {
        Token tagNameToken = advance();
        auto element = std::make_shared<ElementNode>();
        element->tagName = tagNameToken.lexeme;

        if (match({TokenType::LEFT_BRACE})) {
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                // Check for attributes or child nodes
                if ((peek().type == TokenType::IDENTIFIER) && peekNext().type == TokenType::COLON) {
                    parseAttributes(*element);
                } else if (peek().type == TokenType::KEYWORD_STYLE && peekNext().type == TokenType::LEFT_BRACE) {
                    element->styleBlock = parseStyleBlock();
                }
                else if (peek().type == TokenType::KEYWORD_TEXT) {
                    element->children.push_back(parseText());
                } else if (peek().type == TokenType::IDENTIFIER) {
                     element->children.push_back(parseElement());
                } else {
                    // Unknown token inside element block, skip for now
                    advance();
                }
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
        }
        return element;
    }
    return nullptr; // Not an element
}

void Parser::parseAttributes(ElementNode& element) {
    Token name = advance(); // We already know it's an IDENTIFIER or KEYWORD_STYLE
    consume(TokenType::COLON, "Expect ':' after attribute name.");

    Token value;
    if (match({TokenType::STRING, TokenType::UNQUOTED_LITERAL, TokenType::NUMBER})) {
        value = previous();
    } else {
        // Handle error: invalid attribute value
        // For now, just consume token and move on
        value = advance();
    }

    element.attributes.push_back({name.lexeme, value.lexeme});

    if(match({TokenType::SEMICOLON})) {
        // optional semicolon
    }
}


NodePtr Parser::parseText() {
    consume(TokenType::KEYWORD_TEXT, "Expect 'text' keyword.");
    auto textNode = std::make_shared<TextNode>();

    if (match({TokenType::LEFT_BRACE})) {
        if (match({TokenType::STRING, TokenType::UNQUOTED_LITERAL})) {
            textNode->text = previous().lexeme;
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    } else {
        // Handle attribute-style text: text: "some text";
        consume(TokenType::COLON, "Expect ':' after 'text' keyword for attribute style.");
         if (match({TokenType::STRING, TokenType::UNQUOTED_LITERAL})) {
            textNode->text = previous().lexeme;
        }
        if(match({TokenType::SEMICOLON})) {
            // optional semicolon
        }
    }

    // For now, we wrap the text in an ElementNode with tag "text"
    // to fit the simple model. This should be improved.
    // A better approach would be to have a proper TextNode in the AST.
    // I already have TextNode, so this is fine.

    return textNode;
}


// --- Helper Methods ---

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::peekNext() {
    if (tokens.size() <= 1 || current + 1 >= tokens.size()) {
        return tokens.back(); // Should be EOF
    }
    return tokens[current + 1];
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    // Handle error
    std::cerr << "Parse Error: " << message << std::endl;
    // A simple panic mode error recovery would be to throw an exception
    // or to synchronize to the next statement.
    // For now, just return the current token.
    return peek();
}

std::shared_ptr<StyleBlockNode> Parser::parseStyleBlock() {
    consume(TokenType::KEYWORD_STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style' keyword.");

    auto styleBlock = std::make_shared<StyleBlockNode>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Look ahead to see if we're parsing a property or a selector rule.
        // This is a simple heuristic: if we see a '{' before a ';', it's a rule.
        // This is not foolproof, but good enough for now.
        int lookahead = current;
        bool isRule = false;
        while(lookahead < tokens.size() && tokens[lookahead].type != TokenType::SEMICOLON && tokens[lookahead].type != TokenType::RIGHT_BRACE) {
            if (tokens[lookahead].type == TokenType::LEFT_BRACE) {
                isRule = true;
                break;
            }
            lookahead++;
        }

        if (isRule) {
            // Parse a selector rule
            auto rule = std::make_shared<CssRuleNode>();
            std::string selectorText;
            while(peek().type != TokenType::LEFT_BRACE && !isAtEnd()) {
                selectorText += advance().lexeme;
            }
            rule->selector = selectorText;
            consume(TokenType::LEFT_BRACE, "Expect '{' after selector.");
            while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                std::string propName;
                while(peek().type != TokenType::COLON && !isAtEnd()) { propName += advance().lexeme; }
                consume(TokenType::COLON, "Expect ':' after property name.");
                std::string propValue;
                while(peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) { propValue += advance().lexeme; }
                rule->properties.push_back({propName, propValue});
                if (peek().type == TokenType::SEMICOLON) advance();
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after selector block.");
            styleBlock->rules.push_back(rule);
        } else {
            // Parse an inline property
            std::string propName;
            while(peek().type != TokenType::COLON && !isAtEnd()) {
                propName += advance().lexeme;
            }
            consume(TokenType::COLON, "Expect ':' after property name.");
            std::string propValue;
            while(peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
                propValue += advance().lexeme;
            }
            styleBlock->inlineProperties.push_back({propName, propValue});
            if (peek().type == TokenType::SEMICOLON) {
                advance();
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return styleBlock;
}

} // namespace CHTL
