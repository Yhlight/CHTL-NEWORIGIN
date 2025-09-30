#include "StatementState.h"

#include "../CHTLParser/Parser.h"
#include <algorithm>
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/ConditionalNode.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StaticStyleNode.h"
#include "../Util/NodeCloner.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLLoader/ModuleResolver.h"
#include <stdexcept>
#include <string>
#include <filesystem>
#include <utility> // For std::pair
#include <sstream>
#include <regex>

// Forward declare to resolve circular dependency with StyleBlockState.h
class StyleBlockState;
class ConfigurationState;
class UseState;
#include "StyleBlockState.h"
#include "ConfigurationState.h"
#include "UseState.h"
#include "InfoState.h"
#include "CHTL/CHTLStrategy/ElementParsingStrategy.h"

// Forward declare to resolve circular dependency between element parsing and statement parsing
class ElementNode;


// The main handler for this state. It acts as a dispatcher.
std::unique_ptr<BaseNode> StatementState::handle(Parser& parser) {
    if (parser.currentToken.type == TokenType::OpenBracket) {
        const auto& nextValue = parser.peekToken.value;
        if (nextValue == "Origin") {
            return parseOriginDefinition(parser);
        }
        if (nextValue == "Namespace") {
            parseNamespaceDefinition(parser);
            return nullptr;
        }
        if (nextValue == "Configuration") {
            parser.advanceTokens(); // [
            parser.advanceTokens(); // Configuration
            parser.expectToken(TokenType::CloseBracket);
            parser.setState(std::make_unique<ConfigurationState>());
            return nullptr;
        }
        if (nextValue == "Info") {
             parser.advanceTokens(); // [
            parser.advanceTokens(); // Info
            parser.expectToken(TokenType::CloseBracket);
            parser.setState(std::make_unique<InfoState>());
            return nullptr;
        }
        // Fallback for any other bracketed expression that isn't handled above.
        // This is now an error because all valid bracketed statements should be
        // handled by the main Parser loop.
        throw std::runtime_error("Unexpected token in brackets: " + nextValue);

    } else if (parser.currentToken.type == TokenType::Use) {
        parser.setState(std::make_unique<UseState>());
        return nullptr;
    } else if (parser.currentToken.type == TokenType::At) {
        return parseElementTemplateUsage(parser);
    } else if (parser.currentToken.type == TokenType::Identifier) {
        if (parser.currentToken.value == "text") {
            return parseTextElement(parser);
        }
        if (parser.currentToken.value == "script") {
            return parseScriptElement(parser);
        }
        ElementParsingStrategy strategy;
        return strategy.parse(parser);
    } else if (parser.currentToken.type == TokenType::If) {
        return parseIfStatement(parser);
    } else if (parser.currentToken.type == TokenType::HashComment) {
        return parseComment(parser);
    }

    throw std::runtime_error("Statements must begin with '[', an identifier, or hash comment. Found '" + parser.currentToken.value + "' instead.");
}

std::unique_ptr<BaseNode> StatementState::parseIfStatement(Parser& parser) {
    auto conditionalNode = std::make_unique<ConditionalNode>();

    parser.expectToken(TokenType::If);
    parser.expectToken(TokenType::OpenBrace);
    {
        ConditionalCase ifCase;
        parser.expectKeyword(TokenType::Identifier, "condition", "condition");
        parser.expectToken(TokenType::Colon);

        StyleBlockState tempStyleState;
        ifCase.condition = std::move(tempStyleState.parseStyleExpression(parser));
        if (parser.currentToken.type == TokenType::Semicolon) {
            parser.advanceTokens();
        }

        while (parser.currentToken.type != TokenType::CloseBrace) {
            auto childNode = handle(parser);
            if (childNode) ifCase.children.push_back(std::move(childNode));
        }
        conditionalNode->cases.push_back(std::move(ifCase));
    }
    parser.expectToken(TokenType::CloseBrace);

    while (parser.currentToken.type == TokenType::Else && parser.peekToken.type == TokenType::If) {
        parser.advanceTokens(); // else
        parser.advanceTokens(); // if
        parser.expectToken(TokenType::OpenBrace);
        {
            ConditionalCase elseIfCase;
            parser.expectKeyword(TokenType::Identifier, "condition", "condition");
            parser.expectToken(TokenType::Colon);

            StyleBlockState tempStyleState;
            elseIfCase.condition = std::move(tempStyleState.parseStyleExpression(parser));
            if (parser.currentToken.type == TokenType::Semicolon) {
                 parser.advanceTokens();
            }

            while (parser.currentToken.type != TokenType::CloseBrace) {
                auto childNode = handle(parser);
                if (childNode) elseIfCase.children.push_back(std::move(childNode));
            }
            conditionalNode->cases.push_back(std::move(elseIfCase));
        }
        parser.expectToken(TokenType::CloseBrace);
    }

    if (parser.currentToken.type == TokenType::Else) {
        parser.advanceTokens(); // else
        parser.expectToken(TokenType::OpenBrace);
        {
            ConditionalCase elseCase;
            elseCase.condition = std::make_unique<StaticStyleNode>("true");

            while (parser.currentToken.type != TokenType::CloseBrace) {
                auto childNode = handle(parser);
                if (childNode) elseCase.children.push_back(std::move(childNode));
            }
            conditionalNode->cases.push_back(std::move(elseCase));
        }
        parser.expectToken(TokenType::CloseBrace);
    }

    return conditionalNode;
}


std::unique_ptr<BaseNode> StatementState::parseTextElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    std::stringstream textContent;
    bool firstToken = true;
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (!firstToken) {
            textContent << " ";
        }
        textContent << parser.currentToken.value;
        parser.advanceTokens();
        firstToken = false;
    }

    parser.expectToken(TokenType::CloseBrace);
    return std::make_unique<TextNode>(textContent.str());
}

std::unique_ptr<BaseNode> StatementState::parseComment(Parser& parser) {
    auto comment = std::make_unique<CommentNode>(parser.currentToken.value);
    parser.expectToken(TokenType::HashComment);
    return comment;
}

std::unique_ptr<BaseNode> StatementState::parseElementTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    parser.expectKeyword(TokenType::Identifier, "Element", "Element");

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    std::string ns = parser.getCurrentNamespace();
    if (parser.currentToken.type == TokenType::From) {
        parser.advanceTokens();
        std::stringstream ns_builder;
        ns_builder << parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        while(parser.currentToken.type == TokenType::Dot) {
            parser.advanceTokens();
            ns_builder << "." << parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);
        }
        ns = ns_builder.str();
    }

    ElementTemplateNode* tmpl = parser.templateManager.getElementTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Element template not found: " + templateName);
    }

    auto fragment = std::make_unique<FragmentNode>();
    for (const auto& child : tmpl->children) {
        auto clonedNode = NodeCloner::clone(child.get());
        if (clonedNode->sourceTemplateName.empty()) {
            clonedNode->sourceTemplateName = templateName;
        }
        fragment->children.push_back(std::move(clonedNode));
    }

    parser.expectToken(TokenType::Semicolon);

    return fragment;
}

std::unique_ptr<BaseNode> StatementState::parseOriginDefinition(Parser& parser) {
    // Simplified for now to avoid compilation errors
    parser.advanceTokens(); // [
    parser.advanceTokens(); // Origin
    parser.advanceTokens(); // ]
    parser.advanceTokens(); // @
    parser.advanceTokens(); // Type
    parser.advanceTokens(); // name
    parser.advanceTokens(); // {
    while(parser.currentToken.type != TokenType::CloseBrace) {
        parser.advanceTokens();
    }
    parser.advanceTokens(); // }
    return nullptr;
}


void StatementState::parseNamespaceDefinition(Parser& parser) {
    // Simplified for now
    parser.advanceTokens();
    parser.advanceTokens();
    parser.advanceTokens();
    parser.advanceTokens();
    parser.advanceTokens();
    while(parser.currentToken.type != TokenType::CloseBrace) {
        parser.advanceTokens();
    }
    parser.advanceTokens();
}

std::unique_ptr<BaseNode> StatementState::parseScriptElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // script
    parser.expectToken(TokenType::OpenBrace);

    // 1. Capture the raw script content
    std::stringstream content_ss;
    size_t start_pos = parser.currentToken.start_pos;
    size_t end_pos = start_pos;
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        end_pos = parser.currentToken.start_pos + parser.currentToken.value.length();
        parser.advanceTokens();
    }
    std::string scriptContent = parser.lexer.getSource().substr(start_pos, end_pos - start_pos);

    // 2. Scan for auto-injectable selectors
    if (parser.contextNode) {
        std::regex selector_regex(R"(\{\{\s*([.#])([a-zA-Z0-9_-]+)\s*\}\})");
        std::smatch matches;
        std::string::const_iterator search_start(scriptContent.cbegin());

        while (std::regex_search(search_start, scriptContent.cend(), matches, selector_regex)) {
            std::string type = matches[1];
            std::string name = matches[2];

            if (type == "." && parser.contextNode->attributes.find("class") == parser.contextNode->attributes.end()) {
                // Auto-inject class attribute
                parser.contextNode->attributes["class"] = std::make_unique<StaticStyleNode>(name);
            } else if (type == "#" && parser.contextNode->attributes.find("id") == parser.contextNode->attributes.end()) {
                // Auto-inject id attribute
                parser.contextNode->attributes["id"] = std::make_unique<StaticStyleNode>(name);
            }
            search_start = matches.suffix().first;
        }
    }

    parser.expectToken(TokenType::CloseBrace);
    return std::make_unique<ScriptNode>(scriptContent);
}

void StatementState::parseExportBlock(Parser& parser) {
    // Simplified for now
     parser.advanceTokens();
    while(parser.currentToken.type != TokenType::CloseBrace) {
        parser.advanceTokens();
    }
    parser.advanceTokens();
}