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
        while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) {
            consume();
        }
        if (currentToken.type == TokenType::EndOfFile) break;

        if (currentToken.type == TokenType::LeftBracket) {
            parseTemplateDefinition();
        } else {
            NodeList stmts = parseStatement();
            if (!stmts.empty()) {
                nodes.insert(nodes.end(), stmts.begin(), stmts.end());
            } else if (currentToken.type != TokenType::EndOfFile) {
                throw std::runtime_error("Unexpected top-level token: " + currentToken.value);
            }
        }
    }
    return nodes;
}

NodeList Parser::parseStatement() {
    if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text") {
            return {parseText()};
        }
        return {parseElement()};
    } else if (currentToken.type == TokenType::GeneratorComment) {
        return {parseComment()};
    } else if (currentToken.type == TokenType::At) {
        return parseElementUsage();
    }
    return {};
}

NodePtr Parser::parseElement() {
    std::string tagName = currentToken.value;
    eat(TokenType::Identifier);

    auto element = std::make_shared<ElementNode>(tagName);

    eat(TokenType::LeftBrace);

    while (currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) {
            consume();
        }
        if (currentToken.type == TokenType::RightBrace || currentToken.type == TokenType::EndOfFile) break;

        if (currentToken.type == TokenType::Identifier && (peekToken.type == TokenType::Colon || peekToken.type == TokenType::Equal)) {
            parseAttributes(element);
        } else if (currentToken.type == TokenType::Identifier && currentToken.value == "style" && peekToken.type == TokenType::LeftBrace) {
            parseStyleBlock(element);
        } else {
            NodeList children = parseStatement();
            if (!children.empty()) {
                element->children.insert(element->children.end(), children.begin(), children.end());
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

            StyleTemplate final_template = TemplateRegistry::getStyleTemplate(templateName);

            // Specialization
            if (currentToken.type == TokenType::LeftBrace) {
                eat(TokenType::LeftBrace);
                while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
                    if (currentToken.type == TokenType::DeleteKeyword) {
                        eat(TokenType::DeleteKeyword);
                        std::string prop_to_delete = currentToken.value;
                        eat(TokenType::Identifier);
                        final_template.erase(prop_to_delete);
                        eat(TokenType::Semicolon);
                    } else {
                        std::string prop_to_specialize = currentToken.value;
                        eat(TokenType::Identifier);
                        eat(TokenType::Colon);

                        std::vector<Token> value_tokens;
                        while(currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::EndOfFile) {
                            value_tokens.push_back(currentToken);
                            consume();
                        }
                        std::string value = ExpressionParser::parseAndEvaluate(value_tokens);
                        final_template[prop_to_specialize] = value;
                        eat(TokenType::Semicolon);
                    }
                }
                eat(TokenType::RightBrace);
            } else { // Simple usage
                eat(TokenType::Semicolon);
            }

            for (const auto& pair : final_template) {
                if (pair.second.empty()) {
                    throw std::runtime_error("Valueless property '" + pair.first + "' from template '" + templateName + "' was not specialized.");
                }
                style_content << pair.first << ":" << pair.second << ";";
            }

        } else { // Regular style property
            std::string property = currentToken.value;
            eat(TokenType::Identifier);
            eat(TokenType::Colon);
            std::vector<Token> value_tokens;
            while(currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::EndOfFile) {
                value_tokens.push_back(currentToken);
                consume();
            }
            std::string value = ExpressionParser::parseAndEvaluate(value_tokens);
            style_content << property << ":" << value << ";";
            eat(TokenType::Semicolon);
        }
    }

    eat(TokenType::RightBrace);

    if (element->attributes.count("style")) {
        element->attributes["style"] += style_content.str();
    } else {
        element->attributes["style"] = style_content.str();
    }
}

NodePtr Parser::parseTemplateDefinition() {
    eat(TokenType::LeftBracket);
    std::string keyword = currentToken.value;
    if (keyword != "Template" && keyword != "Custom") {
        throw std::runtime_error("Expected 'Template' or 'Custom' keyword in definition.");
    }
    eat(TokenType::Identifier);
    eat(TokenType::RightBracket);

    eat(TokenType::At);
    std::string templateType = currentToken.value;
    eat(TokenType::Identifier); // Consume "Style" or "Element"

    std::string templateName = currentToken.value;
    eat(TokenType::Identifier);

    eat(TokenType::LeftBrace);

    if (templateType == "Style") {
        StyleTemplate style_template;
        while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
            while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) consume();
            if (currentToken.type == TokenType::RightBrace) break;

            std::string property = currentToken.value;
            eat(TokenType::Identifier);

            if (currentToken.type == TokenType::Colon) {
                eat(TokenType::Colon);
                std::vector<Token> value_tokens;
                while(currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::EndOfFile) {
                    value_tokens.push_back(currentToken);
                    consume();
                }
                style_template[property] = ExpressionParser::parseAndEvaluate(value_tokens);
            } else {
                style_template[property] = "";
            }
            eat(TokenType::Semicolon);
        }
        TemplateRegistry::registerStyleTemplate(templateName, style_template);

    } else if (templateType == "Element") {
        NodeList element_template_nodes;
        while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
            while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) consume();
            if (currentToken.type == TokenType::RightBrace) break;

            NodeList stmts = parseStatement();
            if (!stmts.empty()) {
                element_template_nodes.insert(element_template_nodes.end(), stmts.begin(), stmts.end());
            } else {
                throw std::runtime_error("Unexpected token in element template: " + currentToken.value);
            }
        }
        TemplateRegistry::registerElementTemplate(templateName, element_template_nodes);

    } else {
        throw std::runtime_error("Unsupported template type: @" + templateType);
    }

    eat(TokenType::RightBrace);
    return nullptr; // Definitions don't produce a node in the main AST
}

NodeList Parser::parseElementUsage() {
    eat(TokenType::At);
    if (currentToken.value != "Element") {
        throw std::runtime_error("Expected 'Element' keyword for template usage.");
    }
    eat(TokenType::Identifier); // Consume "Element"

    std::string templateName = currentToken.value;
    eat(TokenType::Identifier);

    eat(TokenType::Semicolon);

    NodeList templateNodes = TemplateRegistry::getElementTemplate(templateName);
    NodeList clonedNodes;
    for (const auto& node : templateNodes) {
        clonedNodes.push_back(node->clone());
    }
    return clonedNodes;
}