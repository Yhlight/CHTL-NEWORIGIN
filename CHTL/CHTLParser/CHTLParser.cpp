#include "CHTLParser.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLExpr/ExprParser.h"
#include "../CHTLExpr/LiteralNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/OriginNode.h"
#include <iostream>

const std::string VIRTUAL_ROOT_TAG = "__ROOT__";

CHTLParser::CHTLParser(CHTLLexer& lexer, CHTLContext& context) : lexer(lexer), context(context) {
    consumeToken();
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
    std::string error_message = "Parser Error: " + message + " at line " + std::to_string(currentToken.line) + ", column " + std::to_string(currentToken.column);
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
            auto node = parseDirective();
            if (node) {
                root->addChild(node);
            }
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
        auto node = std::make_shared<CommentNode>(getCurrentToken().value, true);
        consumeToken();
        return node;
    } else if (currentToken.type == TokenType::KEYWORD_USE) {
        consumeToken();
        expect(TokenType::KEYWORD_HTML5, "Expected 'html5' after 'use'.");
        expect(TokenType::SEMICOLON, "Expected ';' after 'use html5'.");
        context.useHtml5Doctype = true;
        return nullptr;
    } else if (currentToken.type == TokenType::AT) {
        return parseTemplateUsage();
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
        switch (currentToken.type) {
            case TokenType::KEYWORD_STYLE:
                parseStyleBlock(element);
                break;
            case TokenType::KEYWORD_TEXT:
                element->addChild(parseTextNode());
                break;
            case TokenType::IDENTIFIER:
                parseIdentifierInBlock(element);
                break;
            case TokenType::HASH_COMMENT:
                element->addChild(std::make_shared<CommentNode>(currentToken.value, true));
                consumeToken();
                break;
            case TokenType::AT:
                element->addChild(parseTemplateUsage());
                break;
            case TokenType::LEFT_BRACKET: {
                auto node = parseDirective();
                if (node) element->addChild(node);
                break;
            }
            case TokenType::RIGHT_BRACE:
                // Loop will terminate
                break;
            default:
                error("Unexpected token inside element block: " + currentToken.value);
        }
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' to end a block.");
    state.leave();
}

void CHTLParser::parseIdentifierInBlock(const std::shared_ptr<ElementNode>& element) {
    Token peek = peekNextToken();
    if (peek.type == TokenType::COLON || peek.type == TokenType::EQUAL) {
        element->addAttribute(parseAttribute());
    } else {
        element->addChild(parseElement());
    }
}

std::shared_ptr<AttributeNode> CHTLParser::parseAttribute() {
    std::string key = currentToken.value;
    consumeToken(); // consume attribute name

    if (currentToken.type == TokenType::COLON || currentToken.type == TokenType::EQUAL) {
        consumeToken(); // consume ':' or '='
    } else {
        error("Expected ':' or '=' after attribute name '" + key + "'.");
    }

    state.enter(ParseState::EXPECTING_ATTRIBUTE_VALUE);

    if (currentToken.type != TokenType::STRING && currentToken.type != TokenType::IDENTIFIER && currentToken.type != TokenType::NUMBER) {
        error("Expected a value for the attribute '" + key + "'.");
    }
    std::string value = currentToken.value;
    consumeToken(); // consume value

    state.leave(); // leave EXPECTING_ATTRIBUTE_VALUE

    expect(TokenType::SEMICOLON, "Expected ';' after attribute value for '" + key + "'.");

    return std::make_shared<AttributeNode>(key, value);
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplateUsage() {
    expect(TokenType::AT, "Expected '@'");
    if (currentToken.type == TokenType::KEYWORD_ELEMENT) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected template name.");
        std::string templateName = currentToken.value;
        consumeToken();
        expect(TokenType::SEMICOLON, "Expected ';' after element template usage.");
        return std::make_shared<TemplateUsageNode>(templateName, TemplateType::Element);
    } else {
        error("Unsupported template usage @" + currentToken.value + " in this context.");
        return nullptr;
    }
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
    return nullptr;
}

void CHTLParser::parseStyleBlock(const std::shared_ptr<ElementNode>& element) {
    expect(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    expect(TokenType::LEFT_BRACE, "Expected '{' for style block.");
    state.enter(ParseState::IN_STYLE_BLOCK);
    while (getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
        switch(getCurrentToken().type) {
            case TokenType::AT: {
                consumeToken();
                expect(TokenType::KEYWORD_STYLE, "Expected 'Style' after '@'");
                std::string templateName = getCurrentToken().value;
                expect(TokenType::IDENTIFIER, "Expected template name.");

                auto usageNode = std::make_shared<TemplateUsageNode>(templateName, TemplateType::Style);
                element->styleUsages.push_back(usageNode);

                expect(TokenType::SEMICOLON, "Expected ';'");
                break;
            }
            case TokenType::DOT:
            case TokenType::HASH: {
                auto ruleNode = std::make_shared<CssRuleNode>();
                TokenType selectorType = getCurrentToken().type;
                consumeToken();
                if (getCurrentToken().type != TokenType::IDENTIFIER) error("Expected selector name.");
                std::string selectorName = getCurrentToken().value;
                consumeToken();
                ruleNode->selector = (selectorType == TokenType::DOT ? "." : "#") + selectorName;
                std::string attrToUpdate = (selectorType == TokenType::DOT) ? "class" : "id";
                bool attrFound = false;
                for (const auto& attr : element->attributes) {
                    if (attr->key == attrToUpdate) {
                        attr->value += (attr->value.empty() ? "" : " ") + selectorName;
                        attrFound = true;
                        break;
                    }
                }
                if (!attrFound) {
                    element->addAttribute(std::make_shared<AttributeNode>(attrToUpdate, selectorName));
                }
                expect(TokenType::LEFT_BRACE, "Expected '{' for selector rule block.");
                ruleNode->properties = parseCssRuleProperties();
                expect(TokenType::RIGHT_BRACE, "Expected '}' for selector rule block.");
                context.globalCssRuleNodes.push_back(ruleNode);
                break;
            }
            case TokenType::IDENTIFIER: {
                std::string key = getCurrentToken().value;
                consumeToken();
                expect(TokenType::COLON, "Expected ':' after style property name.");
                ExprParser exprParser(*this, context);
                auto valueExpr = exprParser.parse();
                element->inlineStyles[key] = valueExpr;
                if (getCurrentToken().type == TokenType::SEMICOLON) {
                    consumeToken();
                }
                break;
            }
            case TokenType::RIGHT_BRACE:
                // loop will terminate
                break;
            default:
                error("Expected a style property, selector, or template usage.");
        }
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' to end style block.");
    state.leave();
}

std::shared_ptr<BaseNode> CHTLParser::parseDirective() {
    expect(TokenType::LEFT_BRACKET, "Expected '[' to start a directive.");
    if (currentToken.type == TokenType::KEYWORD_TEMPLATE) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Template' keyword.");
        parseTemplateDefinition();
        return nullptr;
    } else if (currentToken.type == TokenType::KEYWORD_CUSTOM) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Custom' keyword.");
        parseCustomDefinition();
        return nullptr;
    }
    else if (currentToken.type == TokenType::KEYWORD_ORIGIN) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Origin' keyword.");
        return parseOriginBlock();
    } else {
        error("Unsupported directive found: " + currentToken.value);
        return nullptr;
    }
}

void CHTLParser::parseCustomDefinition() {
    expect(TokenType::AT, "Expected '@' for custom type.");

    if (currentToken.type == TokenType::KEYWORD_STYLE) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the style custom.");
        std::string customName = currentToken.value;
        consumeToken();
        expect(TokenType::LEFT_BRACE, "Expected '{' for custom block.");
        auto customNode = std::make_shared<CustomDefinitionNode>(customName, CustomType::Style);
        while(getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
            if (peekNextToken().type == TokenType::COLON) { // Property with value
                std::string key = getCurrentToken().value;
                consumeToken();
                expect(TokenType::COLON, "Expected ':' after style property name.");
                ExprParser exprParser(*this, context);
                customNode->styleProperties[key] = exprParser.parse();
                if (getCurrentToken().type == TokenType::SEMICOLON) {
                    consumeToken();
                }
            } else { // Valueless property list
                 while(getCurrentToken().type != TokenType::SEMICOLON && getCurrentToken().type != TokenType::RIGHT_BRACE) {
                    if (getCurrentToken().type != TokenType::IDENTIFIER) error("Expected identifier for valueless property.");
                    customNode->valuelessProperties.push_back(getCurrentToken().value);
                    consumeToken();
                    if (getCurrentToken().type == TokenType::COMMA) {
                        consumeToken();
                    } else {
                        break;
                    }
                }
                expect(TokenType::SEMICOLON, "Expected ';' to end property definition.");
                consumeToken();
            }
        }
        context.styleCustoms[customName] = customNode;
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end custom block.");

    } else if (currentToken.type == TokenType::KEYWORD_ELEMENT) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the element custom.");
        std::string customName = currentToken.value;
        consumeToken();
        auto customNode = std::make_shared<CustomDefinitionNode>(customName, CustomType::Element);
        expect(TokenType::LEFT_BRACE, "Expected '{' for custom block.");
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            customNode->children.push_back(parseStatement());
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end custom block.");
        context.elementCustoms[customName] = customNode;

    } else if (currentToken.type == TokenType::KEYWORD_VAR) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the var custom.");
        std::string customName = currentToken.value;
        consumeToken();
        auto customNode = std::make_shared<CustomDefinitionNode>(customName, CustomType::Var);
        expect(TokenType::LEFT_BRACE, "Expected '{' for custom block.");
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            if(currentToken.type != TokenType::IDENTIFIER) error("Expected a variable name.");
            std::string key = currentToken.value;
            consumeToken();
            expect(TokenType::COLON, "Expected ':' after variable name.");
            if (currentToken.type != TokenType::STRING && currentToken.type != TokenType::IDENTIFIER && currentToken.type != TokenType::NUMBER) {
                error("Expected a value for the variable.");
            }
            customNode->variables[key] = currentToken.value;
            consumeToken();
            expect(TokenType::SEMICOLON, "Expected ';' after variable value.");
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end custom block.");
        context.varCustoms[customName] = customNode;
    } else {
        error("Unsupported custom type found: @" + currentToken.value);
    }
}

std::shared_ptr<OriginNode> CHTLParser::parseOriginBlock() {
    expect(TokenType::AT, "Expected '@' for origin type.");
    std::string type = getCurrentToken().value;
    consumeToken();

    expect(TokenType::LEFT_BRACE, "Expected '{' for origin block.");

    const std::string& source = lexer.getSource();
    size_t startPos = lexer.getPosition();
    size_t currentPos = startPos;
    int brace_level = 1;

    while (brace_level > 0 && currentPos < source.length()) {
        if (source[currentPos] == '{') {
            brace_level++;
        } else if (source[currentPos] == '}') {
            brace_level--;
        }
        currentPos++;
    }

    if (brace_level != 0) {
        error("Unmatched braces in origin block.");
    }

    std::string rawContent = source.substr(startPos, currentPos - startPos - 1);
    lexer.setPosition(currentPos);
    consumeToken();

    return std::make_shared<OriginNode>(type, rawContent);
}

void CHTLParser::parseTemplateDefinition() {
    expect(TokenType::AT, "Expected '@' for template type.");

    if (currentToken.type == TokenType::KEYWORD_STYLE) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the style template.");
        std::string templateName = currentToken.value;
        consumeToken();
        expect(TokenType::LEFT_BRACE, "Expected '{' for template block.");
        auto templateNode = std::make_shared<TemplateDefinitionNode>(templateName, TemplateType::Style);
        while(getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
            if (getCurrentToken().type == TokenType::KEYWORD_INHERIT || getCurrentToken().type == TokenType::AT) {
                if (getCurrentToken().type == TokenType::KEYWORD_INHERIT) consumeToken();
                expect(TokenType::AT, "Expected '@'");
                expect(TokenType::KEYWORD_STYLE, "Expected 'Style'");
                std::string parentName = getCurrentToken().value;
                expect(TokenType::IDENTIFIER, "Expected parent template name.");
                if(context.styleTemplates.count(parentName)) {
                    for(const auto& pair : context.styleTemplates[parentName]->styleProperties) {
                        templateNode->styleProperties[pair.first] = pair.second;
                    }
                } else {
                    error("Undefined parent style template: " + parentName);
                }
                expect(TokenType::SEMICOLON, "Expected ';'");
            } else if (getCurrentToken().type == TokenType::IDENTIFIER) {
                std::string key = getCurrentToken().value;
                consumeToken();
                expect(TokenType::COLON, "Expected ':'");
                ExprParser exprParser(*this, context);
                templateNode->styleProperties[key] = exprParser.parse();
                if(getCurrentToken().type == TokenType::SEMICOLON) consumeToken();
            } else {
                error("Expected style property or inheritance.");
            }
        }
        context.styleTemplates[templateName] = templateNode;
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end template block.");

    } else if (currentToken.type == TokenType::KEYWORD_ELEMENT) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the element template.");
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
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the var template.");
        std::string templateName = currentToken.value;
        consumeToken();
        auto templateNode = std::make_shared<TemplateDefinitionNode>(templateName, TemplateType::Var);
        expect(TokenType::LEFT_BRACE, "Expected '{' for template block.");
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            if(currentToken.type != TokenType::IDENTIFIER) error("Expected a variable name.");
            std::string key = currentToken.value;
            consumeToken();
            expect(TokenType::COLON, "Expected ':' after variable name.");
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

std::map<std::string, std::shared_ptr<BaseExprNode>> CHTLParser::parseCssRuleProperties() {
    std::map<std::string, std::shared_ptr<BaseExprNode>> properties;
    while(getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
        if(getCurrentToken().type != TokenType::IDENTIFIER) error("Expected a style property name.");
        std::string key = getCurrentToken().value;
        consumeToken();
        expect(TokenType::COLON, "Expected ':' after style property name.");
        ExprParser exprParser(*this, context);
        auto valueExpr = exprParser.parse();
        properties[key] = valueExpr;
        if (getCurrentToken().type == TokenType::SEMICOLON) {
            consumeToken();
        }
    }
    return properties;
}
