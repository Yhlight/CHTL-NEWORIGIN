#include "CHTLParser.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ElementSpecializationNode.h"
#include "../CHTLNode/DeleteElementNode.h"
#include "../CHTLExpr/ExprParser.h"
#include "../CHTLExpr/LiteralNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ConfigNode.h"
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

    // Check for an optional 'except' clause before the block
    if (currentToken.type == TokenType::KEYWORD_EXCEPT) {
        parseExceptClause(element);
    }

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

        auto usageNode = std::make_shared<TemplateUsageNode>(templateName, TemplateType::Element);

        if (getCurrentToken().type == TokenType::LEFT_BRACE) {
            usageNode->specialization = parseElementSpecializationBlock();
        } else {
            expect(TokenType::SEMICOLON, "Expected ';' or '{' after element template usage.");
        }
        return usageNode;

    } else {
        error("Unsupported template usage @" + currentToken.value + " in this context.");
        return nullptr;
    }
}

std::shared_ptr<ElementSpecializationNode> CHTLParser::parseElementSpecializationBlock() {
    auto specNode = std::make_shared<ElementSpecializationNode>();
    expect(TokenType::LEFT_BRACE, "Expected '{' to start specialization block.");

    while(getCurrentToken().type != TokenType::RIGHT_BRACE) {
        if (getCurrentToken().type == TokenType::KEYWORD_DELETE) {
            consumeToken(); // consume 'delete'
            if (getCurrentToken().type != TokenType::IDENTIFIER) error("Expected selector for delete instruction.");
            // For now, assume simple tag selector. Indexed selectors to be added later.
            std::string selector = getCurrentToken().value;
            consumeToken();
            specNode->addInstruction(std::make_shared<DeleteElementNode>(selector));
            expect(TokenType::SEMICOLON, "Expected ';' after delete instruction.");
        }
        // TODO: Add parsing for 'insert' and element modification instructions
        else {
            error("Unsupported instruction in element specialization block.");
        }
    }

    expect(TokenType::RIGHT_BRACE, "Expected '}' to end specialization block.");
    return specNode;
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

                if (getCurrentToken().type == TokenType::LEFT_BRACE) {
                    consumeToken(); // consume '{'

                    while (getCurrentToken().type != TokenType::RIGHT_BRACE) {
                        if (getCurrentToken().type == TokenType::KEYWORD_DELETE) {
                            consumeToken(); // consume 'delete'
                            while (getCurrentToken().type != TokenType::SEMICOLON) {
                                if (getCurrentToken().type != TokenType::IDENTIFIER) {
                                    error("Expected property name after 'delete'.");
                                }
                                usageNode->deletedProperties.push_back(getCurrentToken().value);
                                consumeToken();
                                if (getCurrentToken().type == TokenType::COMMA) {
                                    consumeToken();
                                } else if (getCurrentToken().type != TokenType::SEMICOLON) {
                                    error("Expected ',' or ';' after property name in delete list.");
                                }
                            }
                        } else {
                            error("Unsupported keyword in style usage block. Only 'delete' is allowed.");
                        }
                        expect(TokenType::SEMICOLON, "Expected ';' to end statement in style usage block.");
                    }
                    expect(TokenType::RIGHT_BRACE, "Expected '}' to end style usage block.");

                } else {
                    expect(TokenType::SEMICOLON, "Expected ';' or '{' after style usage.");
                }

                element->styleUsages.push_back(usageNode);
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
    }
    else if (currentToken.type == TokenType::KEYWORD_IMPORT) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Import' keyword.");
        return parseImportDirective();
    }
    else if (currentToken.type == TokenType::KEYWORD_NAMESPACE) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Namespace' keyword.");
        return parseNamespaceDirective();
    }
    else if (currentToken.type == TokenType::KEYWORD_CONFIGURATION) {
        consumeToken();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Configuration' keyword.");
        parseConfigurationDirective();
        return nullptr; // Directives that modify context don't return a node to be added to the tree
    } else {
        error("Unsupported directive found: " + currentToken.value);
        return nullptr;
    }
}

void CHTLParser::parseConfigurationDirective() {
    // Check for named config, e.g., [Configuration] @Config Basic
    // For now, we only handle the single, unnamed, active configuration.
    // The logic to handle named configs can be added later.
    if (getCurrentToken().type == TokenType::AT) {
        // This is a named config, skip for now.
        // A more robust implementation would parse and store it.
        consumeToken(); // @
        consumeToken(); // Config
        consumeToken(); // Basic
    }

    expect(TokenType::LEFT_BRACE, "Expected '{' for configuration block.");

    while(getCurrentToken().type != TokenType::RIGHT_BRACE) {
        if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
            // It's a [Name] block
            consumeToken();
            expect(TokenType::KEYWORD_NAME, "Expected 'Name' for name mapping block.");
            expect(TokenType::RIGHT_BRACKET, "Expected ']' after 'Name'.");
            expect(TokenType::LEFT_BRACE, "Expected '{' for [Name] block.");

            while(getCurrentToken().type != TokenType::RIGHT_BRACE) {
                if(getCurrentToken().type != TokenType::IDENTIFIER) error("Expected internal keyword name for mapping.");
                std::string key = getCurrentToken().value;
                consumeToken();
                expect(TokenType::EQUAL, "Expected '=' for name mapping.");

                std::vector<std::string> values;
                if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
                    // List of values
                    consumeToken();
                    while(getCurrentToken().type != TokenType::RIGHT_BRACKET) {
                        if (getCurrentToken().type != TokenType::AT && getCurrentToken().type != TokenType::IDENTIFIER) {
                            error("Expected identifier or @-keyword in name mapping list.");
                        }
                        values.push_back(getCurrentToken().value);
                        consumeToken();
                        if (getCurrentToken().type == TokenType::COMMA) consumeToken();
                    }
                    expect(TokenType::RIGHT_BRACKET, "Expected ']' to close name mapping list.");
                } else {
                    // Single value
                    if (getCurrentToken().type != TokenType::AT && getCurrentToken().type != TokenType::IDENTIFIER) {
                        error("Expected identifier or @-keyword for name mapping.");
                    }
                    values.push_back(getCurrentToken().value);
                    consumeToken();
                }
                context.activeConfig->nameMappings[key] = values;
                expect(TokenType::SEMICOLON, "Expected ';' after name mapping.");
            }
            expect(TokenType::RIGHT_BRACE, "Expected '}' to close [Name] block.");

        } else if (getCurrentToken().type == TokenType::IDENTIFIER) {
            // It's a simple key-value setting
            std::string key = getCurrentToken().value;
            consumeToken();
            expect(TokenType::EQUAL, "Expected '=' for setting.");
            if (getCurrentToken().type != TokenType::IDENTIFIER && getCurrentToken().type != TokenType::STRING && getCurrentToken().type != TokenType::NUMBER) {
                error("Expected a value for the setting.");
            }
            std::string value = getCurrentToken().value;
            consumeToken();
            context.activeConfig->settings[key] = value;
            expect(TokenType::SEMICOLON, "Expected ';' after setting.");
        } else {
            error("Unexpected token in configuration block.");
        }
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' to close configuration block.");
}

void CHTLParser::parseExceptClause(const std::shared_ptr<ElementNode>& element) {
    expect(TokenType::KEYWORD_EXCEPT, "Expected 'except' keyword.");

    do {
        Constraint constraint;
        if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
            consumeToken(); // consume '['
            if (getCurrentToken().type == TokenType::KEYWORD_TEMPLATE) {
                consumeToken(); // consume 'Template'
                if (getCurrentToken().type == TokenType::RIGHT_BRACKET) {
                    constraint.type = ConstraintType::AnyTemplate;
                    constraint.identifier = "Template";
                } else {
                    expect(TokenType::AT, "Expected '@' for typed template constraint.");
                    constraint.type = ConstraintType::TemplateType;
                    constraint.identifier = getCurrentToken().value;
                    consumeToken();
                }
            } else if (getCurrentToken().type == TokenType::KEYWORD_CUSTOM) {
                consumeToken(); // consume 'Custom'
                 if (getCurrentToken().type == TokenType::RIGHT_BRACKET) {
                    constraint.type = ConstraintType::AnyCustom;
                    constraint.identifier = "Custom";
                } else {
                    expect(TokenType::AT, "Expected '@' for typed custom constraint.");
                    constraint.type = ConstraintType::CustomType;
                    constraint.identifier = getCurrentToken().value;
                    consumeToken();
                }
            } else {
                error("Unsupported constraint category.");
            }
            expect(TokenType::RIGHT_BRACKET, "Expected ']' to close constraint category.");
        } else if (getCurrentToken().type == TokenType::AT) {
            consumeToken(); // consume '@'
            if (getCurrentToken().type == TokenType::KEYWORD_HTML) {
                constraint.type = ConstraintType::AnyHtml;
                constraint.identifier = "Html";
                consumeToken();
            } else {
                error("Unsupported @-constraint type.");
            }
        } else if (getCurrentToken().type == TokenType::IDENTIFIER) {
            constraint.type = ConstraintType::ExactTag;
            constraint.identifier = getCurrentToken().value;
            consumeToken();
        } else {
            error("Invalid token in except clause. Expected identifier, '[', or '@'.");
        }

        element->constraints.push_back(constraint);

        if (getCurrentToken().type == TokenType::COMMA) {
            consumeToken();
        } else {
            break;
        }
    } while (true);

    expect(TokenType::SEMICOLON, "Expected ';' to terminate except clause.");
}

std::shared_ptr<NamespaceNode> CHTLParser::parseNamespaceDirective() {
    if (getCurrentToken().type != TokenType::IDENTIFIER) {
        error("Expected a name for the namespace.");
    }
    std::string namespaceName = getCurrentToken().value;
    consumeToken();

    auto node = std::make_shared<NamespaceNode>(namespaceName);
    context.namespaceStack.push_back(namespaceName);

    // Namespaces can be single-line or have a block
    if (getCurrentToken().type == TokenType::LEFT_BRACE) {
        consumeToken(); // consume '{'
        while(getCurrentToken().type != TokenType::RIGHT_BRACE && getCurrentToken().type != TokenType::END_OF_FILE) {
            if (currentToken.type == TokenType::LEFT_BRACKET) {
                auto directive = parseDirective();
                if (directive) node->addChild(directive);
            } else {
                auto statement = parseStatement();
                if(statement) node->addChild(statement);
            }
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' to close namespace block.");
    }
    return node;
}

std::shared_ptr<ImportNode> CHTLParser::parseImportDirective() {
    // This is a complex parser due to the many forms of import.
    // Example: [Import] @Html from "path.html" as MyHtml
    // Example: [Import] [Custom] @Element Box from "./box.chtl"

    ImportCategory category = ImportCategory::None;
    ImportType type = ImportType::Chtl; // Default
    std::optional<std::string> itemToImport;
    std::string path;
    std::optional<std::string> alias;

    // Check for category [Custom], [Template], etc.
    if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
        consumeToken(); // consume '['
        if (getCurrentToken().type == TokenType::KEYWORD_CUSTOM) {
            category = ImportCategory::Custom;
        } else if (getCurrentToken().type == TokenType::KEYWORD_TEMPLATE) {
            category = ImportCategory::Template;
        } else {
            error("Unsupported import category.");
        }
        consumeToken(); // consume category keyword
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after import category.");
    }

    // Check for type @Html, @Element, etc.
    expect(TokenType::AT, "Expected '@' for import type.");
    Token typeToken = getCurrentToken();
    consumeToken(); // consume type keyword

    switch(typeToken.type) {
        case TokenType::KEYWORD_HTML: type = ImportType::Html; break;
        case TokenType::KEYWORD_STYLE: type = ImportType::Style; break;
        case TokenType::KEYWORD_JAVASCRIPT: type = ImportType::JavaScript; break;
        case TokenType::KEYWORD_CHTL: type = ImportType::Chtl; break;
        // The following are for typed CHTL imports
        case TokenType::KEYWORD_ELEMENT:
        case TokenType::KEYWORD_VAR:
            // These are valid, but we handle them below.
            break;
        default:
            error("Invalid @type for import: " + typeToken.value);
    }

    // Check for specific item to import (e.g. "Box" in @Element Box)
    // This is not applicable for @Chtl, @Html, etc.
    if (typeToken.type == TokenType::KEYWORD_ELEMENT || typeToken.type == TokenType::KEYWORD_STYLE || typeToken.type == TokenType::KEYWORD_VAR) {
        if (getCurrentToken().type == TokenType::IDENTIFIER) {
            itemToImport = getCurrentToken().value;
            consumeToken(); // consume item name
        }
    }

    // Expect 'from'
    expect(TokenType::KEYWORD_FROM, "Expected 'from' keyword in import statement.");

    // Expect path
    if (getCurrentToken().type != TokenType::STRING && getCurrentToken().type != TokenType::IDENTIFIER) {
        error("Expected a path string or literal for import.");
    }
    path = getCurrentToken().value;
    consumeToken();

    // Check for optional 'as'
    if (getCurrentToken().type == TokenType::KEYWORD_AS) {
        consumeToken();
        if (getCurrentToken().type != TokenType::IDENTIFIER) {
            error("Expected an alias name after 'as'.");
        }
        alias = getCurrentToken().value;
        consumeToken();
    }

    expect(TokenType::SEMICOLON, "Expected ';' to terminate import statement.");

    auto node = std::make_shared<ImportNode>(type, path);
    node->category = category;
    node->importedItem = itemToImport;
    node->alias = alias;

    return node;
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
