#include "CHTLParser.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLExpr/ExprParser.h"
#include <iostream>

const std::string VIRTUAL_ROOT_TAG = "__ROOT__";

CHTLParser::CHTLParser(CHTLLexer& lexer, CHTLContext& context) : lexer(lexer), context(context) {
    consumeToken(); // Initialize currentToken
}

void CHTLParser::consumeToken() {
    currentToken = lexer.getNextToken();
}

const Token& CHTLParser::getCurrentToken() const {
    return currentToken;
}

Token CHTLParser::peekNextToken() {
    return lexer.peekToken();
}

void CHTLParser::error(const std::string& message) {
    std::string error_message = "Parser Error: " + message +
                                " at line " + std::to_string(currentToken.line) +
                                ", column " + std::to_string(currentToken.column);
    throw std::runtime_error(error_message);
}

void CHTLParser::expect(TokenType type, const std::string& message) {
    if (currentToken.type != type) {
        error(message);
    }
    consumeToken();
}

std::shared_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_shared<ElementNode>(VIRTUAL_ROOT_TAG);
    state.enter(ParseState::GLOBAL_SCOPE);

    while (currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::LEFT_BRACKET) {
            parseDirective();
        } else {
            auto statement = parseStatement();
            if (statement) {
                root->addChild(statement);
            }
        }
    }

    state.leave();
    return root;
}

std::shared_ptr<BaseNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        return parseElement();
    } else if (currentToken.type == TokenType::HASH_COMMENT) {
        return parseCommentNode();
    } else if (currentToken.type == TokenType::AT) {
        // This is where @Element usage goes
        expect(TokenType::AT, "Expected '@'");
        expect(TokenType::KEYWORD_ELEMENT, "Expected 'Element' keyword for template usage.");
        std::string templateName = currentToken.value;
        expect(TokenType::IDENTIFIER, "Expected template name.");
        expect(TokenType::SEMICOLON, "Expected ';' after element template usage.");
        return std::make_shared<TemplateUsageNode>(templateName, TemplateType::Element);
    }
    else {
        error("Unexpected token. Expected an element, a #-comment, or an @Element usage.");
        return nullptr;
    }
}

std::shared_ptr<ElementNode> CHTLParser::parseElement() {
    auto element = std::make_shared<ElementNode>(currentToken.value);
    consumeToken();

    if (currentToken.type == TokenType::LEFT_BRACE) {
        parseBlock(element);
    }

    return element;
}

void CHTLParser::parseBlock(const std::shared_ptr<ElementNode>& element) {
    expect(TokenType::LEFT_BRACE, "Expected '{' to start a block.");
    state.enter(ParseState::IN_ELEMENT_BODY);

    while (currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::KEYWORD_STYLE) {
            parseStyleBlock(element);
        }
        else if (currentToken.type == TokenType::KEYWORD_TEXT) {
            element->addChild(parseTextNode());
        } else if (currentToken.type == TokenType::IDENTIFIER) {
            Token peek = lexer.peekToken();
            if (peek.type == TokenType::COLON || peek.type == TokenType::EQUAL) {
                element->addAttribute(parseAttribute());
            } else {
                element->addChild(parseElement());
            }
        } else if (currentToken.type == TokenType::HASH_COMMENT) {
            element->addChild(parseCommentNode());
        }
        else if (currentToken.type == TokenType::AT) {
            // Handle @Element usage inside a block
             element->addChild(parseStatement());
        }
        else {
            if (currentToken.type == TokenType::RIGHT_BRACE) {
                break;
            }
            error("Unexpected token inside element block: " + currentToken.value);
        }
    }

    expect(TokenType::RIGHT_BRACE, "Expected '}' to end a block.");
    state.leave();
}

std::shared_ptr<AttributeNode> CHTLParser::parseAttribute() {
    std::string key = currentToken.value;
    consumeToken();

    if (currentToken.type == TokenType::COLON || currentToken.type == TokenType::EQUAL) {
        consumeToken();
    } else {
        error("Expected ':' or '=' after attribute name.");
    }

    if (currentToken.type != TokenType::STRING && currentToken.type != TokenType::IDENTIFIER && currentToken.type != TokenType::NUMBER) {
        error("Expected a value for the attribute.");
    }

    std::string value = currentToken.value;
    consumeToken();

    expect(TokenType::SEMICOLON, "Expected ';' after attribute value.");

    return std::make_shared<AttributeNode>(key, value);
}

std::shared_ptr<TextNode> CHTLParser::parseTextNode() {
    expect(TokenType::KEYWORD_TEXT, "Expected 'text' keyword.");
    expect(TokenType::LEFT_BRACE, "Expected '{' for text block.");

    if (currentToken.type != TokenType::STRING) {
        error("Expected a string literal inside text block.");
    }

    std::string content = currentToken.value;
    consumeToken();

    expect(TokenType::RIGHT_BRACE, "Expected '}' to end text block.");

    return std::make_shared<TextNode>(content);
}

std::shared_ptr<CommentNode> CHTLParser::parseCommentNode() {
    std::string content = currentToken.value;
    auto node = std::make_shared<CommentNode>(content, true);
    consumeToken();
    return node;
}

void CHTLParser::parseStyleBlock(const std::shared_ptr<ElementNode>& element) {
    expect(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    expect(TokenType::LEFT_BRACE, "Expected '{' for style block.");
    state.enter(ParseState::IN_STYLE_BLOCK);

    while(getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
        std::cout << "StyleBlock Loop Start: " << getCurrentToken().value << std::endl;
        if (getCurrentToken().type == TokenType::AT) {
            consumeToken();
            expect(TokenType::KEYWORD_STYLE, "Expected 'Style' after '@' for template usage.");

            std::string templateName = currentToken.value;
            expect(TokenType::IDENTIFIER, "Expected template name.");

            if (context.styleTemplates.count(templateName)) {
                auto templateNode = context.styleTemplates[templateName];
                for (const auto& pair : templateNode->styleProperties) {
                    element->inlineStyles[pair.first] = pair.second;
                }
            } else {
                error("Undefined style template: " + templateName);
            }
            expect(TokenType::SEMICOLON, "Expected ';' after style template usage.");

        } else if (currentToken.type == TokenType::IDENTIFIER) {
            std::string key = currentToken.value;
            consumeToken();

            expect(TokenType::COLON, "Expected ':' after style property name.");

            ExprParser exprParser(*this, context);
            auto valueExpr = exprParser.parse();

            element->inlineStyles[key] = valueExpr;

            if (currentToken.type == TokenType::SEMICOLON) {
                consumeToken();
            }
        } else {
            error("Expected a style property or a template usage (@Style).");
        }
    }

    expect(TokenType::RIGHT_BRACE, "Expected '}' to end style block.");
    state.leave();
}

void CHTLParser::parseDirective() {
    expect(TokenType::LEFT_BRACKET, "Expected '[' to start a directive.");

    if (currentToken.type == TokenType::KEYWORD_TEMPLATE) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Template' keyword.");
        parseTemplateDefinition();
    } else {
        error("Unsupported directive found: " + currentToken.value);
    }
}

void CHTLParser::parseTemplateDefinition() {
    expect(TokenType::AT, "Expected '@' for template type.");

    if (currentToken.type == TokenType::KEYWORD_STYLE) {
        consumeToken();

        if (currentToken.type != TokenType::IDENTIFIER) {
            error("Expected a name for the style template.");
        }
        std::string templateName = currentToken.value;
        consumeToken();

        expect(TokenType::LEFT_BRACE, "Expected '{' for template block.");

        auto templateNode = std::make_shared<TemplateDefinitionNode>(templateName, TemplateType::Style);

        // Manually parse properties here, avoiding the problematic helper
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {

            // Handle inheritance
            if (currentToken.type == TokenType::KEYWORD_INHERIT || currentToken.type == TokenType::AT) {
                if (currentToken.type == TokenType::KEYWORD_INHERIT) {
                    consumeToken(); // consume 'inherit'
                }
                expect(TokenType::AT, "Expected '@' for template inheritance.");
                expect(TokenType::KEYWORD_STYLE, "Expected 'Style' for template inheritance.");

                std::string parentTemplateName = currentToken.value;
                expect(TokenType::IDENTIFIER, "Expected parent template name.");

                if (context.styleTemplates.count(parentTemplateName)) {
                    auto parentTemplate = context.styleTemplates[parentTemplateName];
                    for (const auto& pair : parentTemplate->styleProperties) {
                        templateNode->styleProperties[pair.first] = pair.second;
                    }
                } else {
                    error("Attempting to inherit from undefined style template: " + parentTemplateName);
                }
                expect(TokenType::SEMICOLON, "Expected ';' after template inheritance.");
                continue; // Continue to next property/inheritance
            }

            if(currentToken.type != TokenType::IDENTIFIER) error("Expected a style property name.");
            std::string key = currentToken.value;
            consumeToken();

            expect(TokenType::COLON, "Expected ':' after style property name.");

            ExprParser exprParser(*this, context);
            auto valueExpr = exprParser.parse();

            templateNode->styleProperties[key] = valueExpr;

            if (currentToken.type == TokenType::SEMICOLON) {
                consumeToken();
            }
        }

        context.styleTemplates[templateName] = templateNode;

        expect(TokenType::RIGHT_BRACE, "Expected '}' to end template block.");

    } else if (currentToken.type == TokenType::KEYWORD_ELEMENT) {
        consumeToken(); // Consume 'Element'

        if (currentToken.type != TokenType::IDENTIFIER) {
            error("Expected a name for the element template.");
        }
        std::string templateName = currentToken.value;
        consumeToken();

        auto templateNode = std::make_shared<TemplateDefinitionNode>(templateName, TemplateType::Element);

        expect(TokenType::LEFT_BRACE, "Expected '{' for template block.");
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            templateNode->children.push_back(parseStatement());
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end template block.");

        context.elementTemplates[templateName] = templateNode;

    } else if (currentToken.type == TokenType::KEYWORD_VAR) {
        consumeToken(); // Consume 'Var'

        if (currentToken.type != TokenType::IDENTIFIER) {
            error("Expected a name for the var template.");
        }
        std::string templateName = currentToken.value;
        consumeToken();

        auto templateNode = std::make_shared<TemplateDefinitionNode>(templateName, TemplateType::Var);

        expect(TokenType::LEFT_BRACE, "Expected '{' for template block.");
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            if(currentToken.type != TokenType::IDENTIFIER) error("Expected a variable name.");
            std::string key = currentToken.value;
            consumeToken();

            expect(TokenType::COLON, "Expected ':' after variable name.");

            // The value can be a string, number, or unquoted literal
            if (currentToken.type != TokenType::STRING && currentToken.type != TokenType::IDENTIFIER && currentToken.type != TokenType::NUMBER) {
                error("Expected a value for the variable.");
            }
            templateNode->variables[key] = currentToken.value;
            consumeToken();

            expect(TokenType::SEMICOLON, "Expected ';' after variable value.");
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end template block.");

        context.varTemplates[templateName] = templateNode;

    } else {
        error("Unsupported template type found: @" + currentToken.value);
    }
}
