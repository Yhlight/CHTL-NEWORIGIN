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

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (currentToken.type == TokenType::LBracket) {
        return parseTemplateDeclaration();
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
                auto child = parseStatement();
                if (child) {
                    node->addChild(std::move(child));
                } else {
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
        if (currentToken.type == TokenType::LBracket) {
            // Template declarations are non-emitting statements.
            // We parse them to register them in the context, but they don't
            // return a node to be added to the document tree.
            parseStatement();
        } else {
            auto statement = parseStatement();
            if (statement) {
                documentNode->addChild(std::move(statement));
            } else {
                // If we can't parse a statement and it wasn't a non-emitting
                // one, then we should break to avoid an infinite loop.
                break;
            }
        }
    }
    return documentNode;
}