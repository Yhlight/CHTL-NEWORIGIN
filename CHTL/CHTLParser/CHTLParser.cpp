#include "CHTLParser.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLExpr/ExprParser.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLExpr/LiteralNode.h"
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
    // The CHTLState constructor already sets the initial state to GLOBAL_SCOPE.
    // state.enter(ParseState::GLOBAL_SCOPE); // This is redundant.

    while (currentToken.type != TokenType::END_OF_FILE) {
        if (!state.isInState(ParseState::GLOBAL_SCOPE)) {
            error("Attempting to parse a top-level statement while not in global scope.");
        }

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
    return root;
}

std::shared_ptr<BaseNode> CHTLParser::parseStatement() {
    // This function can be called from GLOBAL_SCOPE (for top-level elements)
    // or from within a template definition. We will need to enhance this check
    // when we add a specific state for template definitions.
    if (!state.isInState(ParseState::GLOBAL_SCOPE)) {
        // For now, we allow this for template parsing, but a future refactor will improve this.
    }

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
        switch (state.getCurrentState()) {
            case ParseState::IN_ELEMENT_BODY:
                if (currentToken.type == TokenType::KEYWORD_STYLE) {
                    parseStyleBlock(element);
                } else if (currentToken.type == TokenType::KEYWORD_TEXT) {
                    element->addChild(parseTextNode());
                } else if (currentToken.type == TokenType::IDENTIFIER) {
                    Token peek = peekNextToken();
                    if (peek.type == TokenType::COLON || peek.type == TokenType::EQUAL) {
                        element->addAttribute(parseAttribute());
                    } else {
                        element->addChild(parseElement());
                    }
                } else if (currentToken.type == TokenType::HASH_COMMENT) {
                    element->addChild(parseStatement());
                } else if (currentToken.type == TokenType::AT) {
                    element->addChild(parseStatement());
                } else if (currentToken.type == TokenType::LEFT_BRACKET) {
                    auto node = parseDirective();
                    if (node) element->addChild(node);
                } else {
                    if (currentToken.type == TokenType::RIGHT_BRACE) break;
                    error("Unexpected token inside element block: " + currentToken.value);
                }
                break;
            default:
                error("Unhandled parsing state.");
                break;
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
    return nullptr;
}

void CHTLParser::parseStyleBlock(const std::shared_ptr<ElementNode>& element) {
    expect(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    expect(TokenType::LEFT_BRACE, "Expected '{' for style block.");
    state.enter(ParseState::IN_STYLE_BLOCK);
    while (getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
        if (getCurrentToken().type == TokenType::AT) {
            consumeToken();
            expect(TokenType::KEYWORD_STYLE, "Expected 'Style' after '@'");
            std::string templateName = getCurrentToken().value;
            expect(TokenType::IDENTIFIER, "Expected template name.");
            if (context.styleTemplates.count(templateName)) {
                auto templateNode = context.styleTemplates[templateName];
                for (const auto& pair : templateNode->styleProperties) {
                    element->inlineStyles[pair.first] = pair.second;
                }
            } else {
                error("Undefined style template: " + templateName);
            }
            expect(TokenType::SEMICOLON, "Expected ';'");
        }
        else if (getCurrentToken().type == TokenType::DOT || getCurrentToken().type == TokenType::HASH) {
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
        } else if (getCurrentToken().type == TokenType::IDENTIFIER) {
            std::string key = getCurrentToken().value;
            consumeToken();
            expect(TokenType::COLON, "Expected ':' after style property name.");
            element->inlineStyles[key] = parseStyleValue();
            if (getCurrentToken().type == TokenType::SEMICOLON) {
                consumeToken();
            }
        } else {
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
    } else if (currentToken.type == TokenType::KEYWORD_ORIGIN) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Origin' keyword.");
        return parseOriginBlock();
    } else {
        error("Unsupported directive found: " + currentToken.value);
        return nullptr;
    }
}

#include "../CHTLNode/OriginNode.h" // Make sure this is included

std::shared_ptr<OriginNode> CHTLParser::parseOriginBlock() {
    expect(TokenType::AT, "Expected '@' for origin type.");
    if (getCurrentToken().type != TokenType::KEYWORD_HTML && getCurrentToken().type != TokenType::IDENTIFIER) {
        error("Expected a valid type for the origin block (e.g., @Html).");
    }
    std::string type = getCurrentToken().value;
    consumeToken(); // Consume type, currentToken is now '{'

    if (getCurrentToken().type != TokenType::LEFT_BRACE) {
        error("Expected '{' to begin origin block.");
    }

    // Get the position of the opening brace.
    size_t brace_pos = getCurrentToken().pos;

    // We now scan the raw source string manually.
    const std::string& source = lexer.getSource();
    size_t scan_pos = brace_pos + 1; // Start scanning right after the brace
    int brace_level = 1;

    while (brace_level > 0 && scan_pos < source.length()) {
        if (source[scan_pos] == '{') {
            brace_level++;
        } else if (source[scan_pos] == '}') {
            brace_level--;
        }
        scan_pos++;
    }

    if (brace_level != 0) {
        error("Unmatched braces in origin block.");
    }

    // The content is between the braces.
    size_t content_start = brace_pos + 1;
    size_t content_len = scan_pos - content_start - 1;
    std::string rawContent = source.substr(content_start, content_len);

    // Now, we must move the lexer's state past this entire block.
    lexer.setPosition(scan_pos);

    // And re-sync the parser's token.
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
                templateNode->styleProperties[key] = parseStyleValue();
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
        properties[key] = parseStyleValue();
        if (getCurrentToken().type == TokenType::SEMICOLON) {
            consumeToken();
        }
    }
    return properties;
}

std::shared_ptr<BaseExprNode> CHTLParser::parseStyleValue() {
    lexer.markPosition();

    bool isComplexExpression = false;
    Token lookaheadToken = getCurrentToken();

    while (lookaheadToken.type != TokenType::SEMICOLON && lookaheadToken.type != TokenType::RIGHT_BRACE && lookaheadToken.type != TokenType::END_OF_FILE) {
        switch (lookaheadToken.type) {
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::PERCENT:
            case TokenType::QUESTION_MARK:
            case TokenType::GREATER:
            case TokenType::LESS:
            case TokenType::LOGICAL_AND:
            case TokenType::LOGICAL_OR:
            case TokenType::EQUAL_EQUAL:
            case TokenType::NOT_EQUAL:
                isComplexExpression = true;
                break;
            default:
                break;
        }
        if (isComplexExpression) {
            break;
        }
        lookaheadToken = lexer.getNextToken();
    }

    lexer.rewindToMark();

    if (isComplexExpression) {
        ExprParser exprParser(*this, context);
        return exprParser.parse();
    } else {
        // It's a simple literal value
        std::string literalValue;
        while (currentToken.type != TokenType::SEMICOLON && currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            if (!literalValue.empty()) {
                literalValue += " ";
            }
            literalValue += currentToken.value;
            consumeToken();
        }
        return std::make_shared<LiteralNode>(literalValue);
    }
}

void CHTLParser::parseCustomDefinition() {
    expect(TokenType::AT, "Expected '@' for custom type.");

    if (currentToken.type == TokenType::KEYWORD_STYLE) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the custom style.");
        std::string customName = currentToken.value;
        consumeToken();
        expect(TokenType::LEFT_BRACE, "Expected '{' for custom block.");
        auto customNode = std::make_shared<CustomDefinitionNode>(customName, CustomType::Style);
        // For now, we just parse properties like a template. Specialization comes later.
        while(getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
            if (getCurrentToken().type == TokenType::IDENTIFIER) {
                std::string key = getCurrentToken().value;
                consumeToken();
                expect(TokenType::COLON, "Expected ':'");
                customNode->styleProperties[key] = parseStyleValue();
                if(getCurrentToken().type == TokenType::SEMICOLON) consumeToken();
            } else {
                error("Expected style property.");
            }
        }
        context.customStyleTemplates[customName] = customNode;
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end custom block.");

    } else if (currentToken.type == TokenType::KEYWORD_ELEMENT) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the custom element.");
        std::string customName = currentToken.value;
        consumeToken();
        auto customNode = std::make_shared<CustomDefinitionNode>(customName, CustomType::Element);
        expect(TokenType::LEFT_BRACE, "Expected '{' for custom block.");
        while(currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            customNode->children.push_back(parseStatement());
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' to end custom block.");
        context.customElementTemplates[customName] = customNode;

    } else if (currentToken.type == TokenType::KEYWORD_VAR) {
        consumeToken();
        if (currentToken.type != TokenType::IDENTIFIER) error("Expected a name for the custom var.");
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
        context.customVarTemplates[customName] = customNode;
    } else {
        error("Unsupported custom type found: @" + currentToken.value);
    }
}
