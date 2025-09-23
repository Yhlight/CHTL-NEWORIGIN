#include "StatementState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Util/NodeCloner.h"
#include <stdexcept>
#include <string>
#include <utility> // For std::pair
#include <sstream>

// Forward declare to resolve circular dependency with StyleBlockState.h
class StyleBlockState;
#include "StyleBlockState.h"

// Forward declare to resolve circular dependency between element parsing and statement parsing
class ElementNode;

// The main handler for this state. It acts as a dispatcher.
std::unique_ptr<BaseNode> StatementState::handle(Parser& parser) {
    if (parser.currentToken.type == TokenType::OpenBracket) {
        if (parser.peekToken.value == "Template") {
            parseTemplateDefinition(parser);
            return nullptr; // Template definitions don't produce a node in the main AST
        } else if (parser.peekToken.value == "Origin") {
            return parseOriginDefinition(parser);
        } else if (parser.peekToken.value == "Namespace") {
            parseNamespaceDefinition(parser);
            return nullptr; // Namespace definitions don't produce a node
        }
    } else if (parser.currentToken.type == TokenType::Use) {
        parseUseDirective(parser);
        return nullptr; // `use` directive does not produce a node
    } else if (parser.currentToken.type == TokenType::At) {
        return parseElementTemplateUsage(parser);
    } else if (parser.currentToken.type == TokenType::Identifier) {
        if (parser.currentToken.value == "text") {
            return parseTextElement(parser);
        }
        // Any other identifier is assumed to be an element tag.
        return parseElement(parser);
    } else if (parser.currentToken.type == TokenType::HashComment) {
        return parseComment(parser);
    }

    throw std::runtime_error("Statements must begin with '[', an identifier, or hash comment. Found '" + parser.currentToken.value + "' instead.");
}

// Parses a full element, including its body.
std::unique_ptr<BaseNode> StatementState::parseElement(Parser& parser) {
    auto element = std::make_unique<ElementNode>(parser.currentToken.value);
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    parseElementBody(parser, *element);

    parser.expectToken(TokenType::CloseBrace);
    return element;
}

// Parses the body of an element, dispatching to helpers for attributes, styles, or child nodes.
void StatementState::parseElementBody(Parser& parser, ElementNode& element) {
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style" && parser.peekToken.type == TokenType::OpenBrace) {
            // Set context for the style state and use it to parse the block.
            parser.contextNode = &element;
            StyleBlockState styleState;
            styleState.handle(parser); // This will parse the entire style block.
            parser.contextNode = nullptr; // Reset context.
        } else if (parser.currentToken.type == TokenType::Identifier && (parser.peekToken.type == TokenType::Colon || parser.peekToken.type == TokenType::Equals)) {
            parseAttribute(parser, element);
        } else {
            element.children.push_back(handle(parser));
        }
    }
}

// Parses a 'text { "..." }' block.
std::unique_ptr<BaseNode> StatementState::parseTextElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // consume 'text'
    parser.expectToken(TokenType::OpenBrace);
    std::string textContent = parser.currentToken.value;
    parser.expectToken(TokenType::String);
    parser.expectToken(TokenType::CloseBrace);
    return std::make_unique<TextNode>(textContent);
}

// Parses a '# comment' line.
std::unique_ptr<BaseNode> StatementState::parseComment(Parser& parser) {
    auto comment = std::make_unique<CommentNode>(parser.currentToken.value);
    parser.expectToken(TokenType::HashComment);
    return comment;
}

// Parses an attribute 'key: value;'.
void StatementState::parseAttribute(Parser& parser, ElementNode& element) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    parser.advanceTokens();

    std::string value;
    if (parser.currentToken.type == TokenType::Identifier || parser.currentToken.type == TokenType::String) {
        value = parser.currentToken.value;
        parser.advanceTokens();
    } else {
        throw std::runtime_error("Attribute value for '" + key + "' must be an identifier or a string.");
    }

    parser.expectToken(TokenType::Semicolon);

    if (key == "text") {
        element.children.push_back(std::make_unique<TextNode>(value));
    } else {
        element.attributes[key] = value;
    }
}

void StatementState::parseTemplateDefinition(Parser& parser) {
    // 1. Expect [Template]
    parser.expectToken(TokenType::OpenBracket);
    if (parser.currentToken.value != "Template") throw std::runtime_error("Expected 'Template' keyword after '['.");
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::CloseBracket);

    // 2. Expect @Type
    parser.expectToken(TokenType::At);
    if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected template type (Style, Element, Var) after '@'.");
    std::string templateType = parser.currentToken.value;
    parser.advanceTokens();

    // 3. Parse Template Name
    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    parser.expectToken(TokenType::OpenBrace);

    // Get the current namespace to register the template under.
    std::string currentNs = parser.getCurrentNamespace();

    // 4. Parse block and register with manager
    if (templateType == "Style") {
        auto styleNode = std::make_unique<StyleTemplateNode>();
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Inherit || parser.currentToken.type == TokenType::At) {
                if (parser.currentToken.type == TokenType::Inherit) parser.advanceTokens();
                parser.expectToken(TokenType::At);
                if (parser.currentToken.value != "Style") throw std::runtime_error("Can only inherit from an @Style template.");
                parser.advanceTokens();
                std::string parentName = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
                // For now, assume parent is in the same namespace. Cross-namespace inheritance will be handled with `from`.
                StyleTemplateNode* parentTmpl = parser.templateManager.getStyleTemplate(currentNs, parentName);
                if (!parentTmpl) throw std::runtime_error("Parent style template not found: " + parentName);
                for (const auto& pair : parentTmpl->styles) {
                    styleNode->styles[pair.first] = pair.second;
                }
            } else {
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                std::string value;
                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    value += parser.currentToken.value + " ";
                    parser.advanceTokens();
                }
                if (!value.empty()) value.pop_back();
                styleNode->styles[key] = value;
                if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
            }
        }
        parser.templateManager.addStyleTemplate(currentNs, templateName, std::move(styleNode));
    } else if (templateType == "Element") {
        auto elementNode = std::make_unique<ElementTemplateNode>();
        while (parser.currentToken.type != TokenType::CloseBrace) {
             if (parser.currentToken.type == TokenType::Inherit || parser.currentToken.type == TokenType::At) {
                if (parser.currentToken.type == TokenType::Inherit) parser.advanceTokens();
                parser.expectToken(TokenType::At);
                if (parser.currentToken.value != "Element") throw std::runtime_error("Can only inherit from an @Element template.");
                parser.advanceTokens();
                std::string parentName = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
                ElementTemplateNode* parentTmpl = parser.templateManager.getElementTemplate(currentNs, parentName);
                if (!parentTmpl) throw std::runtime_error("Parent element template not found: " + parentName);
                for (const auto& child : parentTmpl->children) {
                    elementNode->children.push_back(NodeCloner::clone(child.get()));
                }
            } else {
                elementNode->children.push_back(handle(parser));
            }
        }
        parser.templateManager.addElementTemplate(currentNs, templateName, std::move(elementNode));
    } else if (templateType == "Var") {
        auto varNode = std::make_unique<VarTemplateNode>();
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Inherit || parser.currentToken.type == TokenType::At) {
                if (parser.currentToken.type == TokenType::Inherit) parser.advanceTokens();
                parser.expectToken(TokenType::At);
                if (parser.currentToken.value != "Var") throw std::runtime_error("Can only inherit from a @Var template.");
                parser.advanceTokens();
                std::string parentName = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
                VarTemplateNode* parentTmpl = parser.templateManager.getVarTemplate(currentNs, parentName);
                if (!parentTmpl) throw std::runtime_error("Parent var template not found: " + parentName);
                for (const auto& pair : parentTmpl->variables) {
                    varNode->variables[pair.first] = pair.second;
                }
            } else {
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                varNode->variables[key] = parser.currentToken.value;
                parser.expectToken(TokenType::String);
                parser.expectToken(TokenType::Semicolon);
            }
        }
        parser.templateManager.addVarTemplate(currentNs, templateName, std::move(varNode));
    } else {
        throw std::runtime_error("Unknown template type: " + templateType);
    }

    parser.expectToken(TokenType::CloseBrace);
}

std::unique_ptr<BaseNode> StatementState::parseElementTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Element") {
        throw std::runtime_error("Expected 'Element' after '@' for template usage.");
    }
    parser.expectToken(TokenType::Identifier); // consume "Element"

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // Handle optional 'from <namespace>' clause
    std::string ns = parser.getCurrentNamespace();
    if (parser.currentToken.type == TokenType::From) {
        parser.advanceTokens(); // consume 'from'
        ns = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    parser.expectToken(TokenType::Semicolon);

    // Get the template from the manager
    ElementTemplateNode* tmpl = parser.templateManager.getElementTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Element template not found: " + templateName);
    }

    // Create a fragment node to hold the cloned children.
    auto fragment = std::make_unique<FragmentNode>();
    for (const auto& child : tmpl->children) {
        fragment->children.push_back(NodeCloner::clone(child.get()));
    }

    return fragment;
}

void StatementState::parseUseDirective(Parser& parser) {
    parser.expectToken(TokenType::Use);
    if (parser.currentToken.value == "html5") {
        parser.outputHtml5Doctype = true;
        parser.expectToken(TokenType::Identifier);
        parser.expectToken(TokenType::Semicolon);
    } else {
        throw std::runtime_error("Unsupported 'use' directive: " + parser.currentToken.value);
    }
}

std::unique_ptr<BaseNode> StatementState::parseOriginDefinition(Parser& parser) {
    // 1. Expect [Origin] @Type
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Origin);
    parser.expectToken(TokenType::CloseBracket);
    parser.expectToken(TokenType::At);
    // We don't use the type for now, but we must parse it.
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    // 2. Find the raw content
    const std::string& source = parser.lexer.getSource();
    size_t startPos = parser.lexer.getPosition();
    size_t braceLevel = 1;
    size_t currentPos = startPos;

    while (currentPos < source.length()) {
        if (source[currentPos] == '{') {
            braceLevel++;
        } else if (source[currentPos] == '}') {
            braceLevel--;
            if (braceLevel == 0) {
                break; // Found the matching closing brace
            }
        }
        currentPos++;
    }

    if (braceLevel != 0) {
        throw std::runtime_error("Unmatched braces in [Origin] block.");
    }

    std::string rawContent = source.substr(startPos, currentPos - startPos);

    // 3. Manually advance the lexer's position past the raw block
    parser.lexer.setPosition(currentPos);

    // 4. Reset the parser's tokens to the new position
    parser.advanceTokens(); // currentToken is now '}'
    parser.expectToken(TokenType::CloseBrace); // Consume '}'

    return std::make_unique<OriginNode>(rawContent);
}

void StatementState::parseNamespaceDefinition(Parser& parser) {
    // 1. Expect [Namespace] name
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Namespace);
    parser.expectToken(TokenType::CloseBracket);
    std::string ns_name = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // 2. Push namespace onto stack
    parser.namespaceStack.push_back(ns_name);

    // 3. Parse body
    parser.expectToken(TokenType::OpenBrace);
    while(parser.currentToken.type != TokenType::CloseBrace) {
        // A namespace can contain template definitions, other namespaces, etc.
        // We can just recursively call the main handle method.
        handle(parser);
    }
    parser.expectToken(TokenType::CloseBrace);

    // 4. Pop namespace from stack
    parser.namespaceStack.pop_back();
}
