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
#include "../CHTLLoader/Loader.h"
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
        if (nextValue == "Import") {
            parseImportStatement(parser);
            return nullptr;
        }
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
        parseTemplateDefinition(parser);
        return nullptr;

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


void StatementState::parseTemplateDefinition(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    bool isCustom = false;
    if (parser.peekToken.value == "Custom") {
        isCustom = true;
        parser.advanceTokens();
    } else {
        parser.expectKeyword(TokenType::Identifier, "KEYWORD_TEMPLATE", "Template");
    }
    parser.expectToken(TokenType::CloseBracket);

    parser.expectToken(TokenType::At);
    if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected template type (Style, Element, Var) after '@'.");
    std::string templateType = parser.currentToken.value;
    parser.advanceTokens();

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    parser.expectToken(TokenType::OpenBrace);

    std::string currentNs = parser.getCurrentNamespace();

    if (templateType == "Style") {
        auto styleNode = std::make_unique<StyleTemplateNode>();
        styleNode->isCustom = isCustom;
        while (parser.currentToken.type != TokenType::CloseBrace) {
             if (parser.currentToken.type == TokenType::Identifier) {
                std::string key = parser.currentToken.value;
                parser.advanceTokens();
                parser.expectToken(TokenType::Colon);

                StyleBlockState tempStyleState;
                styleNode->styles[key] = std::move(tempStyleState.parseStyleExpression(parser));

                if (parser.currentToken.type == TokenType::Semicolon) {
                    parser.advanceTokens();
                }
            } else {
                 throw std::runtime_error("Unexpected token in @Style definition: " + parser.currentToken.value);
            }
        }
        parser.templateManager.addStyleTemplate(currentNs, templateName, std::move(styleNode));
    } else if (templateType == "Element") {
        auto elementNode = std::make_unique<ElementTemplateNode>();
        elementNode->isCustom = isCustom;
        while (parser.currentToken.type != TokenType::CloseBrace) {
            auto childNode = handle(parser);
            if (childNode) {
                if (childNode->getType() == NodeType::Fragment) {
                    auto* fragment = static_cast<FragmentNode*>(childNode.get());
                    for (auto& grandChild : fragment->children) {
                        elementNode->children.push_back(std::move(grandChild));
                    }
                } else {
                    elementNode->children.push_back(std::move(childNode));
                }
            }
        }
        parser.templateManager.addElementTemplate(currentNs, templateName, std::move(elementNode));
    } else if (templateType == "Var") {
        auto varNode = std::make_unique<VarTemplateNode>();
        varNode->isCustom = isCustom;
        while (parser.currentToken.type != TokenType::CloseBrace) {
            std::string key = parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);
            parser.expectToken(TokenType::Colon);
            varNode->variables[key] = parser.currentToken.value;
            parser.expectToken(TokenType::String);
            parser.expectToken(TokenType::Semicolon);
        }
        parser.templateManager.addVarTemplate(currentNs, templateName, std::move(varNode));
    } else {
        throw std::runtime_error("Unknown template type: " + templateType);
    }

    parser.expectToken(TokenType::CloseBrace);
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

void StatementState::parseImportStatement(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectKeyword(TokenType::Identifier, "KEYWORD_IMPORT", "Import");
    parser.expectToken(TokenType::CloseBracket);

    parser.expectToken(TokenType::At);
    std::string importType = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    parser.expectKeyword(TokenType::From, "KEYWORD_FROM", "from");

    if (parser.currentToken.type != TokenType::String) {
        throw std::runtime_error("Import path must be a string literal.");
    }
    std::string path = parser.currentToken.value;
    parser.advanceTokens();

    if (!path.empty() && path.front() == '"') {
        path.erase(0, 1);
    }
    if (!path.empty() && path.back() == '"') {
        path.pop_back();
    }

    parser.expectToken(TokenType::Semicolon);

    ModuleResolver resolver;
    std::filesystem::path current_file_path(parser.sourcePath);
    std::filesystem::path base_path = !parser.sourcePath.empty() && current_file_path.has_parent_path()
                                          ? current_file_path.parent_path()
                                          : std::filesystem::current_path();
    std::vector<std::filesystem::path> resolved_paths = resolver.resolve(path, base_path);

    if (resolved_paths.empty()) {
        throw std::runtime_error("Failed to resolve import: " + path);
    }

    if (importType == "Chtl") {
        for (const auto& resolved_path : resolved_paths) {
            std::string p_str = resolved_path.string();
            try {
                if (resolved_path.extension() == ".cmod") {
                    auto fileContents = Loader::loadCmod(p_str);
                    std::string module_name = resolved_path.stem().string();
                    std::string source_file_key = "src/" + module_name + ".chtl";
                    if (fileContents.find(source_file_key) == fileContents.end()) {
                        throw std::runtime_error("Main source file not found in CMOD archive: " + source_file_key);
                    }
                    std::string fileContent = fileContents.at(source_file_key);
                    Lexer importLexer(fileContent);
                    Parser importParser(importLexer, p_str);
                    importParser.parse();
                    parser.templateManager.merge(importParser.templateManager);
                } else {
                    std::string fileContent = Loader::loadFile(p_str);
                    Lexer importLexer(fileContent);
                    Parser importParser(importLexer, p_str);
                    importParser.parse();
                    parser.templateManager.merge(importParser.templateManager);
                }
            } catch (const std::runtime_error& e) {
                throw std::runtime_error("Failed to import CHTL module '" + p_str + "': " + e.what());
            }
        }
    } else if (importType == "CJmod") {
        for (const auto& resolved_path : resolved_paths) {
            std::string p_str = resolved_path.string();
            try {
                parser.cjmodManager.load_module(p_str);
            } catch (const std::runtime_error& e) {
                throw std::runtime_error("Failed to load CJMOD module '" + p_str + "': " + e.what());
            }
        }
    }
}

std::unique_ptr<BaseNode> StatementState::parseOriginDefinition(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectKeyword(TokenType::Origin, "KEYWORD_ORIGIN", "Origin");
    parser.expectToken(TokenType::CloseBracket);

    parser.expectToken(TokenType::At);
    std::string type = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    std::string name;
    if (parser.currentToken.type == TokenType::Identifier) {
        name = parser.currentToken.value;
        parser.advanceTokens();
    }

    parser.expectToken(TokenType::OpenBrace);

    // The content of an [Origin] block is raw and should not be tokenized.
    // We will find the matching closing brace by scanning the raw source string.
    const std::string& source = parser.lexer.getSource();
    size_t start_pos = parser.lexer.getPosition(); // Position is now after the opening brace
    size_t scan_pos = start_pos;
    int brace_level = 1;

    while (scan_pos < source.length()) {
        if (source[scan_pos] == '{') {
            brace_level++;
        } else if (source[scan_pos] == '}') {
            brace_level--;
            if (brace_level == 0) {
                break;
            }
        }
        scan_pos++;
    }

    if (brace_level != 0) {
        throw std::runtime_error("Unmatched braces in [Origin] block.");
    }

    std::string content = source.substr(start_pos, scan_pos - start_pos);

    // Manually advance the lexer past the entire raw block.
    parser.lexer.setPosition(scan_pos + 1);

    // Reload the parser's lookahead tokens from the new lexer position.
    parser.advanceTokens();
    parser.advanceTokens();
    parser.advanceTokens();

    auto originNode = std::make_unique<OriginNode>(type, content);
    if (!name.empty()) {
        parser.templateManager.addNamedOrigin(parser.getCurrentNamespace(), name, std::move(originNode));
        return nullptr;
    }

    return originNode;
}


void StatementState::parseNamespaceDefinition(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectKeyword(TokenType::Namespace, "KEYWORD_NAMESPACE", "Namespace");
    parser.expectToken(TokenType::CloseBracket);

    std::string ns = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.pushNamespace(ns);

    // Check for block-style namespace
    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens(); // Consume '{'
        while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            // Recursively call the handle method of the *current* state object
            // to parse statements within the namespace block.
            auto node = this->handle(parser);
            // Top-level statements inside a namespace don't typically produce a node
            // for the parent's AST, so we discard the result.
        }
        parser.expectToken(TokenType::CloseBrace);
        parser.popNamespace(); // Pop the namespace after the block is done.
    }
    // If it's not a block, the namespace applies until the end of the file.
    // The spec is a bit ambiguous here, but a block-based scope is more robust,
    // so we only implement the block-style `[Namespace] name { ... }` for now.
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