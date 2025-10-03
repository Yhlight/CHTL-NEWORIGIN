#include "CHTLParser.h"
#include <stdexcept>
#include <algorithm>
#include <string>

CHTLParser::CHTLParser(const std::string& input, CHTLContext& context, bool discoveryMode)
    : lexer(input), context(context), discoveryMode(discoveryMode) {
    advance();
    advance();
}

void CHTLParser::advance() {
    currentToken = nextToken;
    nextToken = lexer.getNextToken();
}

Token CHTLParser::peek() {
    return nextToken;
}

void CHTLParser::expect(TokenType type) {
    if (currentToken.type == type) {
        advance();
    } else {
        throw std::runtime_error("Unexpected token");
    }
}

#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateStyleNode.h"
#include "../CHTLNode/TemplateElementNode.h"
#include "../CHTLNode/TemplateVarNode.h"
#include "../CHTLNode/CustomStyleNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomVarNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/DeclarationNode.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/InsertNode.h"
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLLoader/CHTLLoader.h"

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    std::unique_ptr<BaseNode> node = nullptr;

    if (currentToken.type == TokenType::LBracket) {
        if (peek().type == TokenType::Identifier) {
            const auto& keyword = peek().value;
            if (keyword == "Template") node = parseTemplateDeclaration();
            else if (keyword == "Custom") node = parseCustomDeclaration();
            else if (keyword == "Import") node = parseImportDeclaration();
            else if (keyword == "Namespace") node = parseNamespaceDeclaration();
            else throw std::runtime_error("Unsupported declaration type inside [].");
        }
    } else if (currentToken.type == TokenType::At) {
        advance(); // consume '@'
        if (currentToken.value == "Element") {
            advance(); // consume 'Element'
            std::string templateName = currentToken.value;
            advance();

            std::string nsPath;
            if (currentToken.type == TokenType::From) {
                advance();
                do {
                    nsPath += currentToken.value;
                    advance();
                    if (currentToken.type == TokenType::Dot) {
                        nsPath += ".";
                        advance();
                    } else break;
                } while (true);
            }

            if (discoveryMode) {
                if (currentToken.type == TokenType::LBrace) {
                    // Consume the specialization block without parsing deeply
                    int brace_level = 1;
                    expect(TokenType::LBrace);
                    while (brace_level > 0 && currentToken.type != TokenType::EndOfFile) {
                        if (currentToken.type == TokenType::LBrace) brace_level++;
                        else if (currentToken.type == TokenType::RBrace) brace_level--;
                        advance();
                    }
                } else {
                    expect(TokenType::Semicolon);
                }
                return std::make_unique<DeclarationNode>("@Element usage");
            }

            // First, try to resolve as a Custom Element
            const CustomElementNode* customTmpl = nsPath.empty() ? context.getCustomElement(templateName) : context.getCustomElement(templateName, nsPath);
            if (customTmpl) {
                auto fragment = std::make_unique<FragmentNode>();
                for (const auto& child : customTmpl->getChildren()) {
                    fragment->addChild(child->clone());
                }

                if (currentToken.type == TokenType::LBrace) {
                    auto modifications = parseSpecializationBlock();
                    applySpecialization(fragment.get(), std::move(modifications));
                } else {
                    expect(TokenType::Semicolon);
                }
                node = std::move(fragment);
            } else {
                // Fallback to a regular Template Element
                const TemplateElementNode* tmpl = nsPath.empty() ? context.getElementTemplate(templateName) : context.getElementTemplate(templateName, nsPath);
                if (!tmpl) {
                    throw std::runtime_error("Element template not found: " + templateName);
                }
                auto fragment = std::make_unique<FragmentNode>();
                for (const auto& child : tmpl->getChildren()) {
                    fragment->addChild(child->clone());
                }
                node = std::move(fragment);
                expect(TokenType::Semicolon); // Regular templates do not have specialization blocks
            }
        } else {
             throw std::runtime_error("Unexpected '@' usage. Only @Element is supported at this level.");
        }
    } else if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text" && peek().type == TokenType::LBrace) node = parseTextNode();
        else if (currentToken.value == "style" && peek().type == TokenType::LBrace) node = parseStyleNode();
        else node = parseElement();
    } else if (currentToken.type == TokenType::Use) {
        node = parseUseStatement();
    } else if (currentToken.type == TokenType::Comment) {
        node = parseCommentNode();
    } else if (currentToken.type == TokenType::If) {
        node = parseIfStatement();
    }

    if (node) {
        // Don't check global constraints on fragments, check them on the fragment's children when they are added.
        if (dynamic_cast<FragmentNode*>(node.get()) == nullptr) {
            const auto& global_constraints = context.getActiveGlobalConstraints();
            if (!global_constraints.empty()) {
                std::string nodeType = node->getNodeType();
                for (const auto& constraint : global_constraints) {
                    if (nodeType.rfind(constraint, 0) == 0) {
                        throw std::runtime_error("Node type '" + nodeType + "' is not allowed due to a global 'except' constraint for '" + constraint + "'.");
                    }
                }
            }
        }
    }

    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseTemplateDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Template"
    expect(TokenType::RBracket);
    expect(TokenType::At);
    std::string templateType = currentToken.value; advance();
    std::string templateName = currentToken.value; advance();

    if (templateType == "Style") {
        auto tmplNode = std::make_unique<TemplateStyleNode>(templateName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            std::string propName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string propValue = currentToken.value; advance();
            expect(TokenType::Semicolon);
            tmplNode->addProperty(propName, propValue);
        }
        expect(TokenType::RBrace);
        context.registerStyleTemplate(std::move(tmplNode));
    } else if (templateType == "Element") {
        auto tmplNode = std::make_unique<TemplateElementNode>(templateName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            auto child = parseStatement();
            if (child) tmplNode->addChild(std::move(child));
            else break;
        }
        expect(TokenType::RBrace);
        context.registerElementTemplate(std::move(tmplNode));
    } else if (templateType == "Var") {
        auto tmplNode = std::make_unique<TemplateVarNode>(templateName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            std::string varName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string varValue = currentToken.value; advance();
            expect(TokenType::Semicolon);
            tmplNode->addVariable(varName, varValue);
        }
        expect(TokenType::RBrace);
        context.registerVarTemplate(std::move(tmplNode));
    }
    return std::make_unique<DeclarationNode>("[Template] @" + templateType);
}

std::unique_ptr<BaseNode> CHTLParser::parseCustomDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Custom"
    expect(TokenType::RBracket);
    expect(TokenType::At);
    std::string customType = currentToken.value; advance();
    std::string customName = currentToken.value; advance();

    if (customType == "Style") {
        auto customNode = std::make_unique<CustomStyleNode>(customName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.type == TokenType::Identifier && currentToken.value == "delete") {
                advance();
                do {
                    customNode->addDeletedProperty(currentToken.value); advance();
                    if (currentToken.type == TokenType::Comma) advance();
                    else break;
                } while (true);
                expect(TokenType::Semicolon);
            } else if (currentToken.type == TokenType::Identifier) {
                if (peek().type == TokenType::Colon || peek().type == TokenType::Assign) {
                    std::string propName = currentToken.value; advance();
                    advance(); // consume ':' or '='
                    std::string propValue = currentToken.value; advance();
                    expect(TokenType::Semicolon);
                    customNode->addProperty(propName, propValue);
                } else { // Valueless properties
                    do {
                        customNode->addValuelessProperty(currentToken.value);
                        advance();
                        if (currentToken.type == TokenType::Comma) {
                            advance();
                        } else {
                            break;
                        }
                    } while (currentToken.type == TokenType::Identifier);
                    expect(TokenType::Semicolon);
                }
            } else {
                throw std::runtime_error("Unexpected token in [Custom] @Style block");
            }
        }
        expect(TokenType::RBrace);
        context.registerCustomStyle(std::move(customNode));
    } else if (customType == "Element") {
        auto customNode = std::make_unique<CustomElementNode>(customName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.value == "delete") {
                advance();
                do {
                    customNode->addDeletedElement(currentToken.value); advance();
                    if (currentToken.type == TokenType::Comma) advance();
                    else break;
                } while (true);
                expect(TokenType::Semicolon);
            } else {
                auto child = parseStatement();
                if (child) customNode->addChild(std::move(child));
                else break;
            }
        }
        expect(TokenType::RBrace);
        context.registerCustomElement(std::move(customNode));
    } else if (customType == "Var") {
        auto customNode = std::make_unique<CustomVarNode>(customName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            std::string varName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string varValue = currentToken.value; advance();
            expect(TokenType::Semicolon);
            customNode->addVariable(varName, varValue);
        }
        expect(TokenType::RBrace);
        context.registerCustomVar(std::move(customNode));
    }
    return std::make_unique<DeclarationNode>("[Custom] @" + customType);
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::Identifier); // "style"
    expect(TokenType::LBrace);
    auto styleNode = std::make_unique<StyleNode>();

    while (currentToken.type != TokenType::RBrace) {
        if (currentToken.type == TokenType::At) {
            advance(); // Consume '@'
            if (currentToken.value != "Style") {
                throw std::runtime_error("Expected 'Style' after '@' in a style block.");
            }
            expect(TokenType::Identifier); // "Style"
            std::string templateName = currentToken.value;
            advance();

            // Handle discovery mode separately to avoid side-effects
            if (discoveryMode) {
                if (currentToken.type == TokenType::From) {
                    advance();
                    while (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Dot) {
                        advance();
                    }
                }
                if (currentToken.type == TokenType::LBrace) {
                    expect(TokenType::LBrace);
                    int brace_level = 1;
                    while (brace_level > 0 && currentToken.type != TokenType::EndOfFile) {
                        if (currentToken.type == TokenType::LBrace) brace_level++;
                        else if (currentToken.type == TokenType::RBrace) brace_level--;
                        advance();
                    }
                } else {
                    expect(TokenType::Semicolon);
                }
                continue;
            }

            // Get namespace path if it exists
            std::string nsPath;
            if (currentToken.type == TokenType::From) {
                advance();
                do {
                    nsPath += currentToken.value; advance();
                    if (currentToken.type == TokenType::Dot) {
                        nsPath += "."; advance();
                    } else break;
                } while (true);
            }

            // Try to resolve as a Custom Style first
            const CustomStyleNode* customTmpl = nsPath.empty() ?
                context.getCustomStyle(templateName) :
                context.getCustomStyle(templateName, nsPath);

            if (customTmpl) {
                for (const auto& prop : customTmpl->getProperties()) {
                    styleNode->addProperty(prop.first, prop.second);
                }
                // Handle specialization block for custom styles
                if (currentToken.type == TokenType::LBrace) {
                    expect(TokenType::LBrace);

                    while (currentToken.type != TokenType::RBrace) {
                        if (currentToken.type == TokenType::Identifier && (peek().type == TokenType::Colon || peek().type == TokenType::Assign)) {
                            std::string propName = currentToken.value;
                            advance();
                            advance(); // Consume ':' or '='
                            std::string propValue = currentToken.value;
                            advance();
                            expect(TokenType::Semicolon);

                            // Add the property, whether it was originally valueless or is a new override/addition.
                            styleNode->addProperty(propName, propValue);
                        } else if (currentToken.type == TokenType::Identifier && currentToken.value == "delete") {
                            advance(); // consume 'delete'
                            do {
                                std::string propToDelete = currentToken.value;
                                advance();
                                styleNode->deleteProperty(propToDelete);

                                if (currentToken.type == TokenType::Comma) {
                                    advance();
                                } else {
                                    break;
                                }
                            } while (currentToken.type == TokenType::Identifier);
                            expect(TokenType::Semicolon);
                        } else {
                            throw std::runtime_error("Unexpected token inside @Style specialization block. Expected 'property: value;' or 'delete'.");
                        }
                    }
                    expect(TokenType::RBrace);
                } else {
                    // If there's no block, there must be a semicolon.
                    // Also, ensure there are no valueless properties that need values.
                    if (!customTmpl->getValuelessProperties().empty()) {
                        throw std::runtime_error("Custom style '" + templateName + "' has valueless properties and requires a specialization block {} to provide values.");
                    }
                    expect(TokenType::Semicolon);
                }
            } else {
                // Fallback to regular Style Template
                const TemplateStyleNode* tmpl = nsPath.empty() ?
                    context.getStyleTemplate(templateName) :
                    context.getStyleTemplate(templateName, nsPath);
                if (!tmpl) {
                    throw std::runtime_error("Style template not found: " + templateName);
                }
                for (const auto& prop : tmpl->getProperties()) {
                    styleNode->addProperty(prop.first, prop.second);
                }
                expect(TokenType::Semicolon);
            }
        } else if (peek().type == TokenType::LBrace) {
            // This is a nested CSS rule, e.g., ".class { ... }"
            std::string selector = currentToken.value;
            advance(); // Consume selector
            auto ruleNode = std::make_unique<CssRuleNode>(selector);

            expect(TokenType::LBrace);
            while (currentToken.type != TokenType::RBrace) {
                std::string propName = currentToken.value;
                advance();
                expect(TokenType::Colon);
                std::string propValue = currentToken.value;
                advance();
                expect(TokenType::Semicolon);
                ruleNode->addProperty(propName, propValue);
            }
            expect(TokenType::RBrace);
            styleNode->addRule(std::move(ruleNode));
        } else {
            // This is an inline property
            std::string propName = currentToken.value;
            advance();
            expect(TokenType::Colon);
            std::string propValue = currentToken.value;
            advance();
            expect(TokenType::Semicolon);
            styleNode->addProperty(propName, propValue);
        }
    }
    expect(TokenType::RBrace);
    return styleNode;
}

void CHTLParser::parseAttribute(ElementNode& node) {
    std::string attrName = currentToken.value; expect(TokenType::Identifier);
    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) advance();
    else throw std::runtime_error("Expected ':' or '=' after attribute name.");
    std::string attrValue = currentToken.value; advance();
    expect(TokenType::Semicolon);
    node.setAttribute(attrName, attrValue);
}

std::unique_ptr<BaseNode> CHTLParser::parseTextNode() {
    expect(TokenType::Identifier);
    expect(TokenType::LBrace);
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance();
    expect(TokenType::RBrace);
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseCommentNode() {
    auto node = std::make_unique<CommentNode>(currentToken.value);
    advance();
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextAttribute() {
    expect(TokenType::Identifier);
    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) advance();
    else throw std::runtime_error("Expected ':' or '=' after 'text' attribute.");
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance();
    expect(TokenType::Semicolon);
    return node;
}

void CHTLParser::addChildWithChecks(ElementNode* parent, std::unique_ptr<BaseNode> child) {
    if (!child) return;

    // Check global constraints
    const auto& global_constraints = context.getActiveGlobalConstraints();
    if (!global_constraints.empty()) {
        std::string childType = child->getNodeType();
        for (const auto& constraint : global_constraints) {
            if (childType.rfind(constraint, 0) == 0) {
                throw std::runtime_error("Node type '" + childType + "' is not allowed due to a global 'except' constraint for '" + constraint + "'.");
            }
        }
    }

    // Check tag name constraints
    if (auto* childElement = dynamic_cast<ElementNode*>(child.get())) {
        const auto& constraints = parent->getConstraints();
        const auto& tagName = childElement->getTagName();
        for (const auto& constraint : constraints) {
            if (tagName == constraint) {
                throw std::runtime_error("Element '" + tagName + "' is not allowed in <" + parent->getTagName() + "> due to an 'except' constraint.");
            }
        }
    }

    // Check type constraints
    const auto& typeConstraints = parent->getTypeConstraints();
    if (!typeConstraints.empty()) {
        std::string childType = child->getNodeType();
        for (const auto& typeConstraint : typeConstraints) {
            if (childType.rfind(typeConstraint, 0) == 0) { // checks if childType starts with typeConstraint
                throw std::runtime_error("Node type '" + childType + "' is not allowed in <" + parent->getTagName() + "> due to an 'except' type constraint for '" + typeConstraint + "'.");
            }
        }
    }

    // Do not add declarations to the children list
    if (dynamic_cast<DeclarationNode*>(child.get()) == nullptr) {
        parent->addChild(std::move(child));
    }
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    if (currentToken.type != TokenType::Identifier) return nullptr;
    auto node = std::make_unique<ElementNode>(currentToken.value);
    advance();

    // Check for an optional index, e.g., div[0]
    if (currentToken.type == TokenType::LBracket) {
        advance(); // consume '['
        if (currentToken.type != TokenType::Identifier || !std::all_of(currentToken.value.begin(), currentToken.value.end(), ::isdigit)) {
            throw std::runtime_error("Expected an integer index inside [].");
        }
        int index = std::stoi(currentToken.value);
        node->setIndex(index);
        advance(); // consume number
        expect(TokenType::RBracket); // consume ']'
    }

    if (currentToken.type == TokenType::LBrace) {
        advance();
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.type == TokenType::Except) {
                advance(); // consume 'except'
                do {
                    std::string constraint_value;
                    bool is_type_constraint = false;

                    if (currentToken.type == TokenType::LBracket) {
                        is_type_constraint = true;
                        constraint_value += "["; advance();
                        constraint_value += currentToken.value; advance(); // e.g., "Template"
                        constraint_value += "]"; expect(TokenType::RBracket);

                        if (currentToken.type == TokenType::At) { // for cases like [Template] @Var
                            constraint_value += " "; // Add space for readability
                            constraint_value += "@"; advance();
                            constraint_value += currentToken.value; advance();
                        }
                    } else if (currentToken.type == TokenType::At) {
                        is_type_constraint = true;
                        constraint_value += "@"; advance();
                        constraint_value += currentToken.value; advance(); // e.g., "Html"
                    } else if (currentToken.type == TokenType::Identifier) {
                        constraint_value = currentToken.value;
                        advance();
                    } else {
                        throw std::runtime_error("Invalid token in 'except' clause. Expected identifier, '@', or '['.");
                    }

                    if (is_type_constraint) {
                        node->addTypeConstraint(constraint_value);
                    } else {
                        node->addConstraint(constraint_value);
                    }

                    if (currentToken.type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
                expect(TokenType::Semicolon);
                continue;
            }

            if (currentToken.type == TokenType::Identifier && currentToken.value == "delete") {
                auto deleteNode = parseDeleteStatement();
                node->addChild(std::move(deleteNode));
                continue;
            }

            if (currentToken.type == TokenType::Identifier && currentToken.value == "insert") {
                auto insertNode = parseInsertStatement();
                node->addChild(std::move(insertNode));
                continue;
            }

            if (currentToken.type == TokenType::Identifier && currentToken.value == "insert") {
                auto insertNode = parseInsertStatement();
                node->addChild(std::move(insertNode));
                continue;
            }

            if (currentToken.type == TokenType::Identifier && (peek().type == TokenType::Colon || peek().type == TokenType::Assign)) {
                if (currentToken.value == "text") {
                    auto textNode = parseTextAttribute();
                    addChildWithChecks(node.get(), std::move(textNode));
                }
                else parseAttribute(*node);
            } else {
                auto pre_parse_token = currentToken;
                auto child = parseStatement();
                if (child) {
                    if (auto* fragment = dynamic_cast<FragmentNode*>(child.get())) {
                        auto children = fragment->takeChildren();
                        for (auto& c : children) {
                            addChildWithChecks(node.get(), std::move(c));
                        }
                    } else {
                        addChildWithChecks(node.get(), std::move(child));
                    }
                }
                else if (pre_parse_token == currentToken) break;
            }
        }
        expect(TokenType::RBrace);
    }
    return node;
}

std::unique_ptr<DocumentNode> CHTLParser::parse() {
    auto documentNode = std::make_unique<DocumentNode>();
    while (currentToken.type != TokenType::EndOfFile) {
        auto pre_parse_token = currentToken;
        auto statement = parseStatement();
        if (statement) {
            if (auto* fragment = dynamic_cast<FragmentNode*>(statement.get())) {
                auto children = fragment->takeChildren();
                for (auto& child : children) {
                    // Global constraints are checked in addChildWithChecks, but DocumentNode doesn't have that.
                    // We must check them manually here.
                    const auto& global_constraints = context.getActiveGlobalConstraints();
                    if (!global_constraints.empty()) {
                        std::string childType = child->getNodeType();
                        for (const auto& constraint : global_constraints) {
                            if (childType.rfind(constraint, 0) == 0) {
                                throw std::runtime_error("Node type '" + childType + "' is not allowed due to a global 'except' constraint for '" + constraint + "'.");
                            }
                        }
                    }
                    if (auto useNode = dynamic_cast<UseNode*>(child.get())) {
                        if (useNode->getDirective() == "html5") {
                            documentNode->setHasHtml5Doctype(true);
                        }
                    } else if (dynamic_cast<DeclarationNode*>(child.get()) == nullptr) {
                         documentNode->addChild(std::move(child));
                    }
                }
            } else {
                bool isEmittingNode = true;
                if (auto useNode = dynamic_cast<UseNode*>(statement.get())) {
                    if (useNode->getDirective() == "html5") {
                        documentNode->setHasHtml5Doctype(true);
                    }
                    isEmittingNode = false;
                } else if (dynamic_cast<DeclarationNode*>(statement.get()) != nullptr) {
                    isEmittingNode = false;
                }

                if (isEmittingNode) {
                    documentNode->addChild(std::move(statement));
                }
            }
        } else if (pre_parse_token == currentToken) {
            break;
        }
    }
    return documentNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseImportDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Import"
    expect(TokenType::RBracket);
    std::string fullType;
    if (currentToken.type == TokenType::LBracket) {
        fullType += "["; advance();
        fullType += currentToken.value; advance();
        fullType += "]"; expect(TokenType::RBracket);
    }
    expect(TokenType::At);
    fullType += "@" + currentToken.value;
    advance();
    std::string entityName;
    if (currentToken.type == TokenType::Identifier) {
        entityName = currentToken.value;
        advance();
    }
    expect(TokenType::From);
    std::string filePath = currentToken.value;
    advance();
    std::string alias;
    if (currentToken.type == TokenType::As) {
        advance();
        alias = currentToken.value;
        advance();
    }
    return std::make_unique<ImportNode>(fullType, entityName, filePath, alias);
}

std::unique_ptr<BaseNode> CHTLParser::parseUseStatement() {
    expect(TokenType::Use);
    std::string directive = currentToken.value;
    advance();
    expect(TokenType::Semicolon);
    return std::make_unique<UseNode>(directive);
}

std::unique_ptr<BaseNode> CHTLParser::parseNamespaceDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Namespace"
    expect(TokenType::RBracket);
    std::string namespaceName = currentToken.value;
    advance();
    context.enterNamespace(namespaceName);
    auto namespaceNode = std::make_unique<NamespaceNode>(namespaceName);

    if (currentToken.type == TokenType::LBrace) {
        advance();
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.type == TokenType::Except) {
                advance(); // consume 'except'
                do {
                    std::string constraint_value;
                    if (currentToken.type == TokenType::LBracket) {
                        constraint_value += "["; advance();
                        constraint_value += currentToken.value; advance(); // e.g., "Template"
                        constraint_value += "]"; expect(TokenType::RBracket);
                    } else {
                        throw std::runtime_error("Global 'except' only supports type constraints like [Template].");
                    }
                    context.addGlobalConstraint(constraint_value);
                    if (currentToken.type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
                expect(TokenType::Semicolon);
                continue;
            }

            auto pre_parse_token = currentToken;
            auto child = parseStatement();
            if (child) {
                // Do not add declarations to the children list
                if (dynamic_cast<DeclarationNode*>(child.get()) == nullptr) {
                    namespaceNode->addChild(std::move(child));
                }
            } else if (pre_parse_token == currentToken) {
                break;
            }
        }
        expect(TokenType::RBrace);
    }
    context.leaveNamespace();
    return namespaceNode;
}

std::unique_ptr<IfNode> CHTLParser::parseIfStatement() {
    expect(TokenType::If);
    expect(TokenType::LBrace);

    if (currentToken.type != TokenType::Identifier || currentToken.value != "condition") {
        throw std::runtime_error("Expected 'condition' keyword in if block.");
    }
    advance(); // consume 'condition'
    expect(TokenType::Colon);

    std::vector<Token> conditionTokens;
    while(currentToken.type != TokenType::Comma && currentToken.type != TokenType::RBrace) {
        conditionTokens.push_back(currentToken);
        advance();
    }
     if (currentToken.type == TokenType::Comma) {
        advance();
    }

    auto ifNode = std::make_unique<IfNode>(conditionTokens);

    while (currentToken.type != TokenType::RBrace) {
        std::string propName = currentToken.value;
        advance();
        expect(TokenType::Colon);
        std::string propValue;
        while(currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::RBrace) {
            propValue += currentToken.value;
            if (peek().type != TokenType::Semicolon && peek().type != TokenType::RBrace) {
                propValue += " ";
            }
            advance();
        }
        if (currentToken.type == TokenType::Semicolon) {
            advance();
        }
        ifNode->addProperty(propName, propValue);
    }
    expect(TokenType::RBrace);

    if (currentToken.type == TokenType::Else) {
        advance();
        if (currentToken.type == TokenType::If) {
            ifNode->setElseBranch(parseIfStatement());
        } else {
            expect(TokenType::LBrace);
            auto elseNode = std::make_unique<IfNode>(std::vector<Token>{}); // Empty condition for else
            while (currentToken.type != TokenType::RBrace) {
                std::string propName = currentToken.value;
                advance();
                expect(TokenType::Colon);
                std::string propValue;
                while(currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::RBrace) {
                    propValue += currentToken.value;
                    if (peek().type != TokenType::Semicolon && peek().type != TokenType::RBrace) {
                        propValue += " ";
                    }
                    advance();
                }
                if (currentToken.type == TokenType::Semicolon) {
                    advance();
                }
                elseNode->addProperty(propName, propValue);
            }
            expect(TokenType::RBrace);
            ifNode->setElseBranch(std::move(elseNode));
        }
    }

    return ifNode;
}

std::unique_ptr<ElementNode> CHTLParser::parseElementTarget() {
    if (currentToken.type != TokenType::Identifier) {
        return nullptr;
    }
    auto node = std::make_unique<ElementNode>(currentToken.value);
    advance();

    if (currentToken.type == TokenType::LBracket) {
        advance(); // consume '['
        if (currentToken.type != TokenType::Identifier || !std::all_of(currentToken.value.begin(), currentToken.value.end(), ::isdigit)) {
            throw std::runtime_error("Expected an integer index inside [].");
        }
        int index = std::stoi(currentToken.value);
        node->setIndex(index);
        advance(); // consume number
        expect(TokenType::RBracket); // consume ']'
    }
    return node;
}

std::unique_ptr<InsertNode> CHTLParser::parseInsertStatement() {
    expect(TokenType::Identifier); // "insert"

    InsertPosition position;
    if (currentToken.value == "after") {
        position = InsertPosition::After;
        advance();
    } else if (currentToken.value == "before") {
        position = InsertPosition::Before;
        advance();
    } else if (currentToken.value == "replace") {
        position = InsertPosition::Replace;
        advance();
    } else if (currentToken.value == "at") {
        advance();
        if (currentToken.value == "top") {
            position = InsertPosition::AtTop;
            advance();
        } else if (currentToken.value == "bottom") {
            position = InsertPosition::AtBottom;
            advance();
        } else {
            throw std::runtime_error("Expected 'top' or 'bottom' after 'at'.");
        }
    } else {
        throw std::runtime_error("Invalid position for insert statement. Expected 'after', 'before', 'replace', or 'at'.");
    }

    std::unique_ptr<ElementNode> target = nullptr;
    if (position == InsertPosition::After || position == InsertPosition::Before || position == InsertPosition::Replace) {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a target element for insert statement.");
        }
        target = parseElementTarget();
        if (!target) {
            throw std::runtime_error("Failed to parse target element for insert statement.");
        }
    }

    auto insertNode = std::make_unique<InsertNode>(position, std::move(target));
    expect(TokenType::LBrace);
    while (currentToken.type != TokenType::RBrace) {
        auto child = parseStatement();
        if (child) {
            if (auto* fragment = dynamic_cast<FragmentNode*>(child.get())) {
                auto children = fragment->takeChildren();
                for (auto& c : children) {
                    insertNode->addChild(std::move(c));
                }
            } else {
                insertNode->addChild(std::move(child));
            }
        } else {
            break;
        }
    }
    expect(TokenType::RBrace);

    return insertNode;
}

std::unique_ptr<DeleteNode> CHTLParser::parseDeleteStatement() {
    expect(TokenType::Identifier); // "delete"

    if (currentToken.type == TokenType::At) {
        advance(); // consume '@'
        if (currentToken.value != "Element") {
            throw std::runtime_error("Expected 'Element' after '@' in a delete statement.");
        }
        advance(); // consume 'Element'
        std::string templateName = currentToken.value;
        advance();
        expect(TokenType::Semicolon);
        return std::make_unique<DeleteNode>(templateName);
    } else if (currentToken.type == TokenType::Identifier) {
        // This is a bit tricky. We want to parse just the element name and optional index,
        // not its body. So we create a temporary element node.
        auto target = std::make_unique<ElementNode>(currentToken.value);
        advance();
        if (currentToken.type == TokenType::LBracket) {
            advance(); // consume '['
            if (currentToken.type != TokenType::Identifier || !std::all_of(currentToken.value.begin(), currentToken.value.end(), ::isdigit)) {
                throw std::runtime_error("Expected an integer index inside [].");
            }
            int index = std::stoi(currentToken.value);
            target->setIndex(index);
            advance(); // consume number
            expect(TokenType::RBracket); // consume ']'
        }
        expect(TokenType::Semicolon);
        return std::make_unique<DeleteNode>(std::move(target));
    } else {
        throw std::runtime_error("Invalid token after 'delete'. Expected element or template.");
    }
}

std::unique_ptr<ElementNode> CHTLParser::parseSpecializationBlock() {
    auto node = std::make_unique<ElementNode>("__specialization_block__");
    expect(TokenType::LBrace);
    while (currentToken.type != TokenType::RBrace) {
        if (currentToken.type == TokenType::Identifier && currentToken.value == "delete") {
            auto deleteNode = parseDeleteStatement();
            node->addChild(std::move(deleteNode));
        } else if (currentToken.type == TokenType::Identifier && currentToken.value == "insert") {
            auto insertNode = parseInsertStatement();
            node->addChild(std::move(insertNode));
        } else {
            auto child = parseElement();
            if (child) {
                node->addChild(std::move(child));
            } else {
                break; // Error or EOF
            }
        }
    }
    expect(TokenType::RBrace);
    return node;
}

void CHTLParser::applySpecialization(FragmentNode* fragment, std::unique_ptr<ElementNode> modifications) {
    auto modChildren = modifications->takeChildren();
    auto& targetChildren = const_cast<std::vector<std::unique_ptr<BaseNode>>&>(fragment->getChildren());

    std::map<std::string, int> targetElementCounters;

    for (auto& modChild : modChildren) {
        if (auto* deleteNode = dynamic_cast<DeleteNode*>(modChild.get())) {
            if (deleteNode->getIsTemplateDeletion()) {
                throw std::runtime_error("Deletion of inherited templates is not yet implemented.");
            }

            const auto* target = deleteNode->getTargetElement();
            if (!target) continue;

            const std::string& targetTagName = target->getTagName();
            int targetIndex = target->getIndex();

            if (targetIndex != -1) { // Indexed deletion
                int currentIndexOfTag = 0;
                auto it = targetChildren.begin();
                bool found = false;
                while (it != targetChildren.end()) {
                    if (auto* pElem = dynamic_cast<ElementNode*>(it->get())) {
                        if (pElem->getTagName() == targetTagName) {
                            if (currentIndexOfTag == targetIndex) {
                                targetChildren.erase(it);
                                found = true;
                                break;
                            }
                            currentIndexOfTag++;
                        }
                    }
                    ++it;
                }
                if (!found) {
                    throw std::runtime_error("Could not find element to delete with specified index.");
                }
            } else { // Non-indexed deletion, remove all matches
                targetChildren.erase(
                    std::remove_if(targetChildren.begin(), targetChildren.end(),
                                   [&](const std::unique_ptr<BaseNode>& p) {
                                       if (auto* pElem = dynamic_cast<ElementNode*>(p.get())) {
                                           return pElem->getTagName() == targetTagName;
                                       }
                                       return false;
                                   }),
                    targetChildren.end()
                );
            }
        } else if (auto* insertNode = dynamic_cast<InsertNode*>(modChild.get())) {
            auto newChildren = insertNode->takeChildren();
            auto position = insertNode->getPosition();
            auto* target = insertNode->getTarget();

            if (position == InsertPosition::AtTop) {
                targetChildren.insert(targetChildren.begin(), std::make_move_iterator(newChildren.begin()), std::make_move_iterator(newChildren.end()));
            } else if (position == InsertPosition::AtBottom) {
                targetChildren.insert(targetChildren.end(), std::make_move_iterator(newChildren.begin()), std::make_move_iterator(newChildren.end()));
            } else {
                auto it = targetChildren.end();
                const std::string& targetTagName = target->getTagName();
                int targetIndex = target->getIndex();
                int currentIndexOfTag = 0;

                for (auto current_it = targetChildren.begin(); current_it != targetChildren.end(); ++current_it) {
                    if (auto* pElem = dynamic_cast<ElementNode*>(current_it->get())) {
                        if (pElem->getTagName() == targetTagName) {
                            if (currentIndexOfTag == targetIndex) {
                                it = current_it;
                                break;
                            }
                            currentIndexOfTag++;
                        }
                    }
                }

                if (it == targetChildren.end()) {
                    throw std::runtime_error("Could not find target for insert operation.");
                }

                if (position == InsertPosition::After) {
                    targetChildren.insert(std::next(it), std::make_move_iterator(newChildren.begin()), std::make_move_iterator(newChildren.end()));
                } else if (position == InsertPosition::Before) {
                    targetChildren.insert(it, std::make_move_iterator(newChildren.begin()), std::make_move_iterator(newChildren.end()));
                } else if (position == InsertPosition::Replace) {
                    it = targetChildren.erase(it);
                    targetChildren.insert(it, std::make_move_iterator(newChildren.begin()), std::make_move_iterator(newChildren.end()));
                }
            }
        } else if (auto* modElement = dynamic_cast<ElementNode*>(modChild.get())) {
            const std::string& modTagName = modElement->getTagName();
            int modIndex = modElement->getIndex();
            ElementNode* targetNodeToModify = nullptr;

            if (modIndex != -1) { // Indexed access
                int currentIndexOfTag = 0;
                for (auto& targetChild : targetChildren) {
                    if (auto* targetElement = dynamic_cast<ElementNode*>(targetChild.get())) {
                        if (targetElement->getTagName() == modTagName) {
                            if (currentIndexOfTag == modIndex) {
                                targetNodeToModify = targetElement;
                                break;
                            }
                            currentIndexOfTag++;
                        }
                    }
                }
            } else { // Ordered access
                int& modTagCount = targetElementCounters[modTagName];
                int currentTargetCount = 0;
                for (auto& targetChild : targetChildren) {
                    if (auto* targetElement = dynamic_cast<ElementNode*>(targetChild.get())) {
                        if (targetElement->getTagName() == modTagName) {
                            if (currentTargetCount == modTagCount) {
                                targetNodeToModify = targetElement;
                                break;
                            }
                            currentTargetCount++;
                        }
                    }
                }
                if (targetNodeToModify) {
                    modTagCount++;
                }
            }

            if (targetNodeToModify) {
                auto innerModChildren = modElement->takeChildren();
                for (auto& innerModChild : innerModChildren) {
                    if (auto* styleNode = dynamic_cast<StyleNode*>(innerModChild.get())) {
                        StyleNode* targetStyleNode = nullptr;
                        for (const auto& c : targetNodeToModify->getChildren()) {
                            if (auto* ts = dynamic_cast<StyleNode*>(c.get())) {
                                targetStyleNode = ts;
                                break;
                            }
                        }
                        if (!targetStyleNode) {
                            auto newStyleNode = std::make_unique<StyleNode>();
                            targetStyleNode = newStyleNode.get();
                            targetNodeToModify->addChild(std::move(newStyleNode));
                        }
                        for (const auto& prop : styleNode->getProperties()) {
                            targetStyleNode->addProperty(prop.first, prop.second);
                        }
                        for (const auto& rule : styleNode->getRules()) {
                            targetStyleNode->addRule(std::unique_ptr<CssRuleNode>(static_cast<CssRuleNode*>(rule->clone().release())));
                        }
                    }
                }
            } else {
                 throw std::runtime_error("Could not find matching element for specialization: " + modTagName);
            }
        }
    }
}