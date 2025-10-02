#include "CHTLParser.h"
#include <stdexcept>

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
#include "../CHTLLoader/CHTLLoader.h"

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::LBracket) {
        if (peek().type == TokenType::Identifier) {
            const auto& keyword = peek().value;
            if (keyword == "Template") return parseTemplateDeclaration();
            if (keyword == "Custom") return parseCustomDeclaration();
            if (keyword == "Import") return parseImportDeclaration();
            if (keyword == "Namespace") return parseNamespaceDeclaration();
        }
        throw std::runtime_error("Unsupported declaration type inside [].");
    }
    if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text" && peek().type == TokenType::LBrace) return parseTextNode();
        if (currentToken.value == "style" && peek().type == TokenType::LBrace) return parseStyleNode();
        return parseElement();
    }
    if (currentToken.type == TokenType::Use) {
        return parseUseStatement();
    }
    if (currentToken.type == TokenType::Comment) return parseCommentNode();
    return nullptr;
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
    return nullptr; // Non-emitting
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
            if (currentToken.value == "delete") {
                advance();
                do {
                    customNode->addDeletedProperty(currentToken.value); advance();
                    if (currentToken.type == TokenType::Comma) advance();
                    else break;
                } while (true);
                expect(TokenType::Semicolon);
            } else {
                std::string propName = currentToken.value; advance();
                expect(TokenType::Colon);
                std::string propValue = currentToken.value; advance();
                expect(TokenType::Semicolon);
                customNode->addProperty(propName, propValue);
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
    return nullptr; // Non-emitting
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::Identifier);
    expect(TokenType::LBrace);
    auto styleNode = std::make_unique<StyleNode>();
    while (currentToken.type != TokenType::RBrace) {
        if (currentToken.type == TokenType::At) {
            advance();
            expect(TokenType::Identifier); // "Style"
            std::string templateName = currentToken.value; advance();
            if (!discoveryMode) {
                const TemplateStyleNode* tmpl = nullptr;
                if (currentToken.type == TokenType::From) {
                    advance();
                    std::string nsPath;
                    do {
                        nsPath += currentToken.value; advance();
                        if (currentToken.type == TokenType::Dot) {
                            nsPath += "."; advance();
                        } else break;
                    } while (true);
                    tmpl = context.getStyleTemplate(templateName, nsPath);
                } else {
                    tmpl = context.getStyleTemplate(templateName);
                }
                if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);
                for (const auto& prop : tmpl->getProperties()) {
                    styleNode->addProperty(prop.first, prop.second);
                }
            } else {
                if (currentToken.type == TokenType::From) {
                    advance();
                    while (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Dot) {
                        advance();
                    }
                }
            }
            expect(TokenType::Semicolon);
        } else {
            std::string propName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string propValue = currentToken.value; advance();
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

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    if (currentToken.type != TokenType::Identifier) return nullptr;
    auto node = std::make_unique<ElementNode>(currentToken.value);
    advance();
    if (currentToken.type == TokenType::LBrace) {
        advance();
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.type == TokenType::Except) {
                advance(); // consume 'except'
                do {
                    if (currentToken.type == TokenType::Identifier) {
                        node->addConstraint(currentToken.value);
                        advance();
                    } else {
                        throw std::runtime_error("Expected an identifier for constraint.");
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

            if (currentToken.type == TokenType::Identifier && (peek().type == TokenType::Colon || peek().type == TokenType::Assign)) {
                if (currentToken.value == "text") node->addChild(parseTextAttribute());
                else parseAttribute(*node);
            } else {
                auto pre_parse_token = currentToken;
                auto child = parseStatement();
                if (child) {
                    if (auto* childElement = dynamic_cast<ElementNode*>(child.get())) {
                        const auto& constraints = node->getConstraints();
                        const auto& tagName = childElement->getTagName();
                        for (const auto& constraint : constraints) {
                            if (tagName == constraint) {
                                throw std::runtime_error("Element '" + tagName + "' is not allowed in <" + node->getTagName() + "> due to an 'except' constraint.");
                            }
                        }
                    }
                    node->addChild(std::move(child));
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
            if (auto useNode = dynamic_cast<UseNode*>(statement.get())) {
                if (useNode->getDirective() == "html5") {
                    documentNode->setHasHtml5Doctype(true);
                }
            }
            documentNode->addChild(std::move(statement));
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
            auto pre_parse_token = currentToken;
            auto child = parseStatement();
            if (child) {
                namespaceNode->addChild(std::move(child));
            } else if (pre_parse_token == currentToken) {
                break;
            }
        }
        expect(TokenType::RBrace);
        context.leaveNamespace();
    }
    return namespaceNode;
}