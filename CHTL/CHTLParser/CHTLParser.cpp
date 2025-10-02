#include "CHTLParser.h"
#include <stdexcept>

CHTLParser::CHTLParser(const std::string& input, CHTLContext& context)
    : lexer(input), context(context) {
    // Initialize both tokens for lookahead
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
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLLoader/CHTLLoader.h"

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::LBracket) {
        if (peek().type == TokenType::Identifier) {
            const auto& keyword = peek().value;
            if (keyword == "Template") {
                return parseTemplateDeclaration();
            } else if (keyword == "Custom") {
                return parseCustomDeclaration();
            } else if (keyword == "Import") {
                return parseImportDeclaration();
            } else if (keyword == "Namespace") {
                return parseNamespaceDeclaration();
            }
        }
        throw std::runtime_error("Unsupported declaration type inside []. Expected 'Template', 'Custom', 'Import', or 'Namespace'.");
    }
    if (currentToken.type == TokenType::Identifier) {
        // The 'text' keyword is special; it can start a text block.
        if (currentToken.value == "text" && peek().type == TokenType::LBrace) {
            return parseTextNode();
        }
        if (currentToken.value == "style" && peek().type == TokenType::LBrace) {
            return parseStyleNode();
        }
        return parseElement();
    }
    if (currentToken.type == TokenType::Comment) {
        return parseCommentNode();
    }
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseTemplateDeclaration() {
    expect(TokenType::LBracket);
    if (currentToken.type != TokenType::Identifier || currentToken.value != "Template") {
        throw std::runtime_error("Expected 'Template' keyword inside [].");
    }
    advance();
    expect(TokenType::RBracket);

    expect(TokenType::At);

    if (currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected template type specifier (e.g., @Style).");
    }

    std::string templateType = currentToken.value;
    advance();

    if (templateType == "Style") {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a name for the style template.");
        }
        std::string templateName = currentToken.value;
        advance();

        auto tmplNode = std::make_unique<TemplateStyleNode>(templateName);

        expect(TokenType::LBrace);

        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected a style property name.");
            }
            std::string propName = currentToken.value;
            advance();

            expect(TokenType::Colon);

            if (currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected a style property value.");
            }
            std::string propValue = currentToken.value;
            advance();

            expect(TokenType::Semicolon);

            tmplNode->addProperty(propName, propValue);
        }

        expect(TokenType::RBrace);
        context.registerStyleTemplate(std::move(tmplNode));
        return nullptr;
    } else if (templateType == "Element") {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a name for the element template.");
        }
        std::string templateName = currentToken.value;
        advance();

        auto tmplNode = std::make_unique<TemplateElementNode>(templateName);

        expect(TokenType::LBrace);

        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            auto child = parseStatement();
            if (child) {
                tmplNode->addChild(std::move(child));
            } else {
                break;
            }
        }

        expect(TokenType::RBrace);
        context.registerElementTemplate(std::move(tmplNode));
        return nullptr;
    } else if (templateType == "Var") {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a name for the var template.");
        }
        std::string templateName = currentToken.value;
        advance();

        auto tmplNode = std::make_unique<TemplateVarNode>(templateName);

        expect(TokenType::LBrace);

        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected a variable name.");
            }
            std::string varName = currentToken.value;
            advance();

            expect(TokenType::Colon);

            if (currentToken.type != TokenType::String) {
                throw std::runtime_error("Expected a string literal for variable value.");
            }
            std::string varValue = currentToken.value;
            advance();

            expect(TokenType::Semicolon);

            tmplNode->addVariable(varName, varValue);
        }

        expect(TokenType::RBrace);
        context.registerVarTemplate(std::move(tmplNode));
        return nullptr;
    } else {
        throw std::runtime_error("Unsupported template type: " + templateType);
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::Identifier); // Consume 'style'
    expect(TokenType::LBrace);

    auto styleNode = std::make_unique<StyleNode>();

    while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::At) {
            advance(); // Consume '@'
            if (currentToken.type != TokenType::Identifier || currentToken.value != "Style") {
                throw std::runtime_error("Expected @Style for template usage.");
            }
            advance(); // Consume 'Style'

            if (currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected template name after @Style.");
            }
            std::string templateName = currentToken.value;
            advance();

            const TemplateStyleNode* tmpl = context.getStyleTemplate(templateName);
            if (!tmpl) {
                throw std::runtime_error("Style template not found: " + templateName);
            }

            for (const auto& prop : tmpl->getProperties()) {
                styleNode->addProperty(prop.first, prop.second);
            }

            expect(TokenType::Semicolon);
        } else if (currentToken.type == TokenType::Identifier) {
            std::string propName = currentToken.value;
            advance();

            expect(TokenType::Colon);

            if (currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected a style property value.");
            }
            std::string propValue = currentToken.value;
            advance();

            expect(TokenType::Semicolon);

            styleNode->addProperty(propName, propValue);
        } else {
             throw std::runtime_error("Unexpected token in style block.");
        }
    }

    expect(TokenType::RBrace);
    return styleNode;
}

void CHTLParser::parseAttribute(ElementNode& node) {
    std::string attrName = currentToken.value;
    expect(TokenType::Identifier);

    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) {
        advance(); // Consume ':' or '='
    } else {
        throw std::runtime_error("Expected ':' or '=' after attribute name.");
    }

    if (currentToken.type != TokenType::String && currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected a string literal or an unquoted literal for attribute value.");
    }
    std::string attrValue = currentToken.value;
    advance(); // Consume string or identifier value

    expect(TokenType::Semicolon);

    node.setAttribute(attrName, attrValue);
}

std::unique_ptr<BaseNode> CHTLParser::parseTextNode() {
    expect(TokenType::Identifier); // Consume 'text'
    expect(TokenType::LBrace);
    if (currentToken.type != TokenType::String) {
         throw std::runtime_error("Expected a string literal inside text block.");
    }
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance(); // Consume string
    expect(TokenType::RBrace);
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseCommentNode() {
    auto node = std::make_unique<CommentNode>(currentToken.value);
    advance(); // Consume comment token
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextAttribute() {
    expect(TokenType::Identifier); // Consume 'text'
    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) {
        advance();
    } else {
        throw std::runtime_error("Expected ':' or '=' after 'text' attribute.");
    }

    if (currentToken.type != TokenType::String) {
        throw std::runtime_error("Expected a string literal for 'text' attribute value.");
    }
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance(); // Consume string value

    expect(TokenType::Semicolon);
    return node;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    if (currentToken.type != TokenType::Identifier) {
        return nullptr;
    }

    auto node = std::make_unique<ElementNode>(currentToken.value);
    advance(); // Consume identifier

    if (currentToken.type == TokenType::LBrace) {
        advance(); // Consume '{'
        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type == TokenType::Identifier && (peek().type == TokenType::Colon || peek().type == TokenType::Assign)) {
                if (currentToken.value == "text") {
                    node->addChild(parseTextAttribute());
                } else {
                    parseAttribute(*node);
                }
            } else {
                auto pre_parse_token = currentToken;
                auto child = parseStatement();
                if (child) {
                    node->addChild(std::move(child));
                } else if (pre_parse_token == currentToken) {
                    // Parser is stuck, break the loop
                    break;
                }
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
            documentNode->addChild(std::move(statement));
        } else if (pre_parse_token == currentToken) {
            // If parseStatement returned nullptr and the token hasn't changed,
            // we are stuck on an unhandled token. Break to avoid an infinite loop.
            break;
        }
        // Otherwise, it was a valid non-emitting statement, and we can continue.
    }
    return documentNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseCustomDeclaration() {
    expect(TokenType::LBracket);
    if (currentToken.type != TokenType::Identifier || currentToken.value != "Custom") {
        throw std::runtime_error("Expected 'Custom' keyword inside [].");
    }
    advance();
    expect(TokenType::RBracket);

    expect(TokenType::At);

    if (currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected custom type specifier (e.g., @Style).");
    }

    std::string customType = currentToken.value;
    advance();

    if (customType == "Style") {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a name for the custom style.");
        }
        std::string customName = currentToken.value;
        advance();

        auto customNode = std::make_unique<CustomStyleNode>(customName);

        expect(TokenType::LBrace);

        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type == TokenType::Identifier && currentToken.value == "delete") {
                advance(); // consume 'delete'
                do {
                    if (currentToken.type != TokenType::Identifier) {
                        throw std::runtime_error("Expected property name after 'delete'.");
                    }
                    customNode->addDeletedProperty(currentToken.value);
                    advance();
                    if (currentToken.type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
                expect(TokenType::Semicolon);
            } else if (currentToken.type == TokenType::Identifier) {
                std::string propName = currentToken.value;
                advance();
                expect(TokenType::Colon);
                if (currentToken.type != TokenType::Identifier) {
                    throw std::runtime_error("Expected a style property value.");
                }
                std::string propValue = currentToken.value;
                advance();
                expect(TokenType::Semicolon);
                customNode->addProperty(propName, propValue);
            } else {
                throw std::runtime_error("Unexpected token in custom style block.");
            }
        }
        expect(TokenType::RBrace);
        context.registerCustomStyle(std::move(customNode));
        return nullptr;
    } else if (customType == "Element") {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a name for the custom element.");
        }
        std::string customName = currentToken.value;
        advance();

        auto customNode = std::make_unique<CustomElementNode>(customName);

        expect(TokenType::LBrace);

        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type == TokenType::Identifier && currentToken.value == "delete") {
                advance(); // consume 'delete'
                do {
                    if (currentToken.type != TokenType::Identifier) {
                        throw std::runtime_error("Expected element name after 'delete'.");
                    }
                    customNode->addDeletedElement(currentToken.value);
                    advance();
                    if (currentToken.type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
                expect(TokenType::Semicolon);
            } else {
                 auto child = parseStatement();
                if (child) {
                    customNode->addChild(std::move(child));
                } else {
                    break;
                }
            }
        }

        expect(TokenType::RBrace);
        context.registerCustomElement(std::move(customNode));
        return nullptr;
    } else if (customType == "Var") {
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected a name for the var custom.");
        }
        std::string customName = currentToken.value;
        advance();

        auto customNode = std::make_unique<CustomVarNode>(customName);

        expect(TokenType::LBrace);

        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected a variable name.");
            }
            std::string varName = currentToken.value;
            advance();

            expect(TokenType::Colon);

            if (currentToken.type != TokenType::String) {
                throw std::runtime_error("Expected a string literal for variable value.");
            }
            std::string varValue = currentToken.value;
            advance();

            expect(TokenType::Semicolon);

            customNode->addVariable(varName, varValue);
        }

        expect(TokenType::RBrace);
    context.registerCustomVar(std::move(customNode));
        return nullptr;
    } else {
        throw std::runtime_error("Unsupported custom type: " + customType);
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseImportDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // Consume "Import"
    expect(TokenType::RBracket);

    std::string fullType;
    if (currentToken.type == TokenType::LBracket) {
        // Handle full spec like [Custom] @Element
        fullType += "[";
        advance();
        fullType += currentToken.value;
        advance();
        fullType += "]";
        expect(TokenType::RBracket);
    }

    expect(TokenType::At);
    fullType += "@" + currentToken.value;
    std::string importType = currentToken.value;
    advance();

    std::string entityName;
    if (currentToken.type == TokenType::Identifier) {
        entityName = currentToken.value;
        advance();
    }

    expect(TokenType::From);
    if (currentToken.type != TokenType::String && currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected a file path string or literal after 'from'.");
    }
    std::string filePath = currentToken.value;
    advance();

    std::string alias;
    if (currentToken.type == TokenType::As) {
        advance();
        if (currentToken.type != TokenType::Identifier) {
            throw std::runtime_error("Expected an alias name after 'as'.");
        }
        alias = currentToken.value;
        advance();
    }

    // For now, we just create the node. The actual loading and processing
    // will be handled by a later component that uses the context.
    return std::make_unique<ImportNode>(fullType, entityName, filePath, alias);
}

std::unique_ptr<BaseNode> CHTLParser::parseNamespaceDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // Consume "Namespace"
    expect(TokenType::RBracket);

    if (currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected a name for the namespace.");
    }
    std::string namespaceName = currentToken.value;
    advance();

    context.enterNamespace(namespaceName);
    auto namespaceNode = std::make_unique<NamespaceNode>(namespaceName);

    if (currentToken.type == TokenType::LBrace) {
        advance(); // Consume '{'
        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            auto pre_parse_token = currentToken;
            auto child = parseStatement();
            if (child) {
                namespaceNode->addChild(std::move(child));
            } else if (pre_parse_token == currentToken) {
                // Parser is stuck, break the loop
                break;
            }
        }
        expect(TokenType::RBrace);
        context.leaveNamespace();
    }
    // If there's no brace, the namespace extends to the end of the file.
    // In that case, we don't call leaveNamespace() here. The calling process
    // would be responsible for managing that at the end of parsing the file.

    return namespaceNode;
}