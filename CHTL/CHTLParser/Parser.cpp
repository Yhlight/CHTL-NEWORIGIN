#include "Parser.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// Constructor and State Management
CHTLParser::CHTLParser(std::vector<Token> tokens) : tokens(std::move(tokens)) {
    states.push_back(ParserState::GLOBAL);
}

CHTLParser::ParserState CHTLParser::currentState() {
    if (states.empty()) return ParserState::GLOBAL;
    return states.back();
}

// Main Parsing Logic
std::vector<std::shared_ptr<BaseNode>> CHTLParser::parse() {
    std::vector<std::shared_ptr<BaseNode>> statements;
    while (!isAtEnd()) {
        try {
            auto new_nodes = parseStatement();
            if(!new_nodes.empty()) {
                statements.insert(statements.end(), new_nodes.begin(), new_nodes.end());
            }
        } catch (const ParseError& error) {
            std::cerr << "Parse Error: " << error.what() << std::endl;
            synchronize();
        }
    }
    return statements;
}

std::vector<std::shared_ptr<BaseNode>> CHTLParser::parseStatement() {
    if (peek().type == TokenType::GENERATOR_COMMENT) {
        return {std::make_shared<CommentNode>(advance().lexeme)};
    }

    if (peek().type == TokenType::LEFT_BRACKET) {
        parseTemplateDefinition();
        return {};
    }

    if (check(TokenType::AT)) {
        advance(); // consume '@'
        Token type = consume(TokenType::IDENTIFIER, "Expected template type.");
        if (type.lexeme == "Element") {
            Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
            consume(TokenType::SEMICOLON, "Expected ';' after element template usage.");
            auto elementTemplate = parseContext.findElementTemplate(name.lexeme);
            if(elementTemplate) {
                std::vector<std::shared_ptr<BaseNode>> cloned_nodes;
                for(const auto& node : elementTemplate->nodes) {
                    cloned_nodes.push_back(node->clone());
                }
                return cloned_nodes;
            } else {
                throw ParseError("Undefined element template: " + name.lexeme);
            }
        }
    }

    if (peek().type == TokenType::IDENTIFIER) {
        if (peek().lexeme == "text") return {parseTextElement()};
        return {parseElement()};
    }

    if(!isAtEnd()) advance(); // Move past unknown tokens to avoid infinite loops
    return {};
}

// Element and Block Parsers
std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expected element name.");
    auto element = std::make_shared<ElementNode>(identifier.lexeme);
    consume(TokenType::LEFT_BRACE, "Expected '{' after element name.");
    states.push_back(ParserState::ELEMENT_BODY);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS) {
            parseAttribute(element);
        } else if (peek().lexeme == "style") {
            parseStyleBlock(element);
        } else {
            auto new_nodes = parseStatement();
            element->children.insert(element->children.end(), new_nodes.begin(), new_nodes.end());
        }
    }

    states.pop_back();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after element block.");
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseTextElement() {
    consume(TokenType::IDENTIFIER, "Expected 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text' keyword.");
    Token content = consume(TokenType::STRING_LITERAL, "Expected string literal inside text block.");
    consume(TokenType::RIGHT_BRACE, "Expected '}' after text block.");
    return std::make_shared<TextNode>(content.lexeme);
}

void CHTLParser::parseAttribute(std::shared_ptr<ElementNode> element) {
    Token key = consume(TokenType::IDENTIFIER, "Expected attribute name.");
    if (!match({TokenType::COLON, TokenType::EQUALS})) throw ParseError("Expected ':' or '=' after attribute name.");
    if (match({TokenType::STRING_LITERAL, TokenType::SINGLE_QUOTED_LITERAL, TokenType::IDENTIFIER, TokenType::UNQUOTED_LITERAL})) {
        element->attributes[key.lexeme] = previous().lexeme;
    } else {
        throw ParseError("Expected attribute value.");
    }
    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
}

void CHTLParser::parseStyleBlock(std::shared_ptr<ElementNode> element) {
    consume(TokenType::IDENTIFIER, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style' keyword.");
    states.push_back(ParserState::STYLE_BLOCK);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::AT)) {
            consume(TokenType::AT, "Expected '@'.");
            Token type = consume(TokenType::IDENTIFIER, "Expected template type, e.g., 'Style'.");
            if (type.lexeme == "Style") {
                Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
                auto styleTemplate = parseContext.findStyleTemplate(name.lexeme);
                if (styleTemplate) {
                    for (const auto& prop : styleTemplate->properties) element->inlineStyles[prop.first] = prop.second;
                } else {
                    throw ParseError("Undefined style template: " + name.lexeme);
                }
                consume(TokenType::SEMICOLON, "Expected ';' after template usage.");
            }
        } else if (check(TokenType::DOT) || check(TokenType::HASH) || check(TokenType::AMPERSAND)) {
            std::string selector;
            if (check(TokenType::AMPERSAND)) {
                advance();
                std::string base_selector;
                if (element->attributes.count("class")) base_selector = "." + element->attributes.at("class");
                else if (element->attributes.count("id")) base_selector = "#" + element->attributes.at("id");
                else throw ParseError("Using '&' selector without a class or id on the parent element.");
                selector = base_selector;
                while(!check(TokenType::LEFT_BRACE) && !isAtEnd()) selector += advance().lexeme;
            } else if (match({TokenType::DOT})) {
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
                rule.properties[key.lexeme] = parseValueUntilSemicolon();
            }
            consume(TokenType::RIGHT_BRACE, "Expected '}' after selector block.");
            element->globalCssRules.push_back(rule);
        } else {
            Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
            consume(TokenType::COLON, "Expected ':' after style property name.");
            element->inlineStyles[key.lexeme] = parseValueUntilSemicolon();
        }
    }
    states.pop_back();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block.");
}

// Template and Value Parsers
std::string CHTLParser::parseValueUntilSemicolon() {
    // Check for variable template usage first, as it's the most specific pattern.
    if (peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::LEFT_PAREN) {
        Token templateName = advance();
        consume(TokenType::LEFT_PAREN, "Expected '(' after variable template name.");
        Token varName = consume(TokenType::IDENTIFIER, "Expected variable name inside parentheses.");
        consume(TokenType::RIGHT_PAREN, "Expected ')' to close variable template usage.");

        auto varTemplate = parseContext.findVarTemplate(templateName.lexeme);
        if (varTemplate && varTemplate->variables.count(varName.lexeme)) {
            // This is a simple substitution, so we assume it's the whole value.
            std::string value = varTemplate->variables.at(varName.lexeme);
            consume(TokenType::SEMICOLON, "Expected ';' after value.");
            return value;
        } else {
            throw ParseError("Undefined variable '" + varName.lexeme + "' in template '" + templateName.lexeme + "'");
        }
    }

    // Check for arithmetic operators
    bool hasOperators = false;
    for(int i = current; tokens[i].type != TokenType::SEMICOLON && i < tokens.size(); ++i) {
        TokenType type = tokens[i].type;
        if (type == TokenType::PLUS || type == TokenType::MINUS || type == TokenType::STAR || type == TokenType::SLASH) {
            hasOperators = true;
            break;
        }
    }

    if (hasOperators) {
        std::vector<Token> expression_tokens;
        std::string expression_unit = "px"; // default

        while(tokens[current].type != TokenType::SEMICOLON && current < tokens.size()) {
            if (tokens[current].type == TokenType::IDENTIFIER) {
                expression_unit = tokens[current].lexeme;
            } else {
                expression_tokens.push_back(tokens[current]);
            }
            current++;
        }

        ExpressionParser exprParser(expression_tokens);
        auto expr = exprParser.parse();
        ExpressionEvaluator evaluator;
        double result = evaluator.evaluate(*expr);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << result;
        std::string s = ss.str();
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        s.erase(s.find_last_not_of('.') + 1, std::string::npos);

        consume(TokenType::SEMICOLON, "Expected ';' after value.");
        return s + expression_unit;
    }

    // Fallback for simple, non-arithmetic, non-variable values
    std::string value = "";
    while(!check(TokenType::SEMICOLON) && !isAtEnd()) {
        Token last_token = peek();
        value += advance().lexeme;
        if (peek().type != TokenType::SEMICOLON) {
            if (last_token.type == TokenType::NUMBER && peek().type == TokenType::IDENTIFIER) {
                // No space needed
            } else {
                value += " ";
            }
        }
    }
    if (!value.empty() && value.back() == ' ') value.pop_back();
    consume(TokenType::SEMICOLON, "Expected ';' after value.");
    return value;
}

void CHTLParser::parseTemplateDefinition() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' to start template definition.");
    consume(TokenType::IDENTIFIER, "Expected 'Template' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expected ']' to end template keyword.");
    consume(TokenType::AT, "Expected '@' for template type.");
    Token type = consume(TokenType::IDENTIFIER, "Expected template type (e.g., 'Style').");

    if (type.lexeme == "Style") {
        Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
        consume(TokenType::LEFT_BRACE, "Expected '{' for template body.");
        StyleTemplate styleTemplate;
        styleTemplate.name = name.lexeme;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::AT) || check(TokenType::KEYWORD_INHERIT)) {
                if (check(TokenType::KEYWORD_INHERIT)) advance();
                consume(TokenType::AT, "Expected '@' for template inheritance.");
                consume(TokenType::IDENTIFIER, "Expected 'Style' keyword for inheritance.");
                Token parentName = consume(TokenType::IDENTIFIER, "Expected parent template name.");
                consume(TokenType::SEMICOLON, "Expected ';' after template inheritance.");
                auto parentTemplate = parseContext.findStyleTemplate(parentName.lexeme);
                if (parentTemplate) {
                    for (const auto& prop : parentTemplate->properties) styleTemplate.properties[prop.first] = prop.second;
                } else {
                    throw ParseError("Undefined parent style template: " + parentName.lexeme);
                }
            } else {
                Token key = consume(TokenType::IDENTIFIER, "Expected style property name.");
                consume(TokenType::COLON, "Expected ':' after style property name.");
                styleTemplate.properties[key.lexeme] = parseValueUntilSemicolon();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");
        parseContext.addStyleTemplate(name.lexeme, std::move(styleTemplate));
    } else if (type.lexeme == "Element") {
        Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
        consume(TokenType::LEFT_BRACE, "Expected '{' for template body.");
        ElementTemplate elementTemplate;
        elementTemplate.name = name.lexeme;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto new_nodes = parseStatement();
            elementTemplate.nodes.insert(elementTemplate.nodes.end(), new_nodes.begin(), new_nodes.end());
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");
        parseContext.addElementTemplate(name.lexeme, std::move(elementTemplate));
    } else if (type.lexeme == "Var") {
        Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
        consume(TokenType::LEFT_BRACE, "Expected '{' for template body.");
        VarTemplate varTemplate;
        varTemplate.name = name.lexeme;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            Token key = consume(TokenType::IDENTIFIER, "Expected variable name.");
            consume(TokenType::COLON, "Expected ':' after variable name.");
            if (match({TokenType::STRING_LITERAL, TokenType::SINGLE_QUOTED_LITERAL, TokenType::IDENTIFIER, TokenType::UNQUOTED_LITERAL})) {
                 varTemplate.variables[key.lexeme] = previous().lexeme;
            } else {
                throw ParseError("Expected variable value.");
            }
            consume(TokenType::SEMICOLON, "Expected ';' after variable value.");
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");
        parseContext.addVarTemplate(name.lexeme, std::move(varTemplate));
    }
}

// Helper Methods
bool CHTLParser::isAtEnd() {
    return current >= tokens.size() || peek().type == TokenType::END_OF_FILE;
}

Token& CHTLParser::peek() {
    return tokens[current];
}

Token& CHTLParser::previous() {
    return tokens[current - 1];
}

Token& CHTLParser::advance() {
    if (!isAtEnd()) current++;
    while(!isAtEnd() && (peek().type == TokenType::SINGLE_LINE_COMMENT || peek().type == TokenType::MULTI_LINE_COMMENT)) {
        current++;
    }
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
            case TokenType::LEFT_BRACE:
            case TokenType::RIGHT_BRACE:
                return;
            default:
                break;
        }
        advance();
    }
}
