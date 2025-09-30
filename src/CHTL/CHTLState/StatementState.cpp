#include "StatementState.h"

#include "../CHTLParser/Parser.h"
#include <algorithm>
#include <vector>
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
            if (parser.tryExpectKeyword(TokenType::Inherit, "KEYWORD_INHERIT", "inherit")) {
                parser.expectToken(TokenType::At);
                parser.expectKeyword(TokenType::Identifier, "KEYWORD_STYLE", "Style");
                styleNode->parentNames.push_back(parser.currentToken.value);
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
            }
            else if (parser.currentToken.type == TokenType::Identifier) {
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

    if (parser.currentToken.type == TokenType::OpenBrace) {
        parseElementSpecializationBlock(parser, *fragment);
    } else {
        parser.expectToken(TokenType::Semicolon);
    }

    return fragment;
}

void StatementState::parseElementSpecializationBlock(Parser& parser, FragmentNode& fragment) {
    parser.expectToken(TokenType::OpenBrace);
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.tryExpectKeyword(TokenType::Delete, "KEYWORD_DELETE", "delete")) {
            parseDeleteInSpecialization(parser, fragment);
        } else if (parser.tryExpectKeyword(TokenType::Insert, "KEYWORD_INSERT", "insert")) {
            parseInsertInSpecialization(parser, fragment);
        } else {
            throw std::runtime_error("Unexpected token in element specialization block: " + parser.currentToken.value);
        }
    }
    parser.expectToken(TokenType::CloseBrace);
}

void StatementState::parseDeleteInSpecialization(Parser& parser, FragmentNode& fragment) {
    std::string tagName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    int index_to_delete = -1; // -1 means delete all matching tags
    if (parser.currentToken.type == TokenType::OpenBracket) {
        parser.advanceTokens(); // consume '['
        index_to_delete = std::stoi(parser.currentToken.value);
        parser.expectToken(TokenType::Number);
        parser.expectToken(TokenType::CloseBracket);
    }

    auto& children = fragment.children;
    int current_index = 0;

    auto it = children.begin();
    while (it != children.end()) {
        bool removed = false;
        if ((*it)->getType() == NodeType::Element) {
            auto* element = static_cast<ElementNode*>((*it).get());
            if (element->tagName == tagName) {
                if (index_to_delete == -1 || current_index == index_to_delete) {
                    it = children.erase(it);
                    removed = true;
                }
                current_index++;
            }
        }
        if (!removed) {
            ++it;
        }
    }

    parser.expectToken(TokenType::Semicolon);
}

void StatementState::parseInsertInSpecialization(Parser& parser, FragmentNode& fragment) {
    // This is a simplified implementation. A full version would need to handle
    // all position keywords and more complex selectors.
    parser.expectKeyword(TokenType::Identifier, "KEYWORD_AFTER", "after");

    // Simplified selector: tag[index]
    std::string tagName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBracket);
    int target_index = std::stoi(parser.currentToken.value);
    parser.expectToken(TokenType::Number);
    parser.expectToken(TokenType::CloseBracket);

    std::vector<std::unique_ptr<BaseNode>> new_nodes;
    parser.expectToken(TokenType::OpenBrace);
    while(parser.currentToken.type != TokenType::CloseBrace) {
        new_nodes.push_back(handle(parser));
    }
    parser.expectToken(TokenType::CloseBrace);

    // Find insertion point
    auto& children = fragment.children;
    int current_index = 0;
    auto insert_it = children.end();
    for (auto it = children.begin(); it != children.end(); ++it) {
        if ((*it)->getType() == NodeType::Element) {
            auto* element = static_cast<ElementNode*>((*it).get());
            if (element->tagName == tagName) {
                if (current_index == target_index) {
                    insert_it = std::next(it);
                    break;
                }
                current_index++;
            }
        }
    }

    if (insert_it != children.end()) {
        children.insert(insert_it, std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
    } else {
        throw std::runtime_error("Could not find insertion point for insert specialization.");
    }
}

// Stub for element modification, which is more complex.
void StatementState::parseElementModificationInSpecialization(Parser& parser, FragmentNode& fragment) {
    // To be implemented
}


void StatementState::parseImportStatement(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectKeyword(TokenType::Identifier, "KEYWORD_IMPORT", "Import");
    parser.expectToken(TokenType::CloseBracket);

    std::string metaQualifier;
    bool isGranular = false;
    if (parser.currentToken.type == TokenType::OpenBracket) {
        isGranular = true;
        parser.advanceTokens();
        metaQualifier = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        parser.expectToken(TokenType::CloseBracket);
    }

    parser.expectToken(TokenType::At);
    std::string importType = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    bool isWildcard = (parser.currentToken.type == TokenType::From);
    std::string itemName;
    if (isGranular && !isWildcard) {
        itemName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    parser.expectKeyword(TokenType::From, "KEYWORD_FROM", "from");

    if (parser.currentToken.type != TokenType::String) {
        throw std::runtime_error("Import path must be a string literal.");
    }
    std::string path = parser.currentToken.value;
    parser.advanceTokens();

    if (!path.empty() && path.front() == '"') path.erase(0, 1);
    if (!path.empty() && path.back() == '"') path.pop_back();

    std::string alias;
    if (parser.tryExpectKeyword(TokenType::Identifier, "KEYWORD_AS", "as")) {
        alias = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
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

    if (importType == "Chtl" && !isGranular) {
        for (const auto& p : resolved_paths) {
            std::string content = Loader::loadFile(p.string());
            Lexer importLexer(content);
            Parser importParser(importLexer, p.string());
            importParser.parse();
            parser.templateManager.merge(importParser.templateManager);
        }
    } else if (importType == "CJmod" && !isGranular) {
        for (const auto& p : resolved_paths) {
            parser.cjmodManager.load_module(p.string());
        }
    } else if (isGranular && !itemName.empty()) {
        std::string resolved_path_str = resolved_paths[0].string();
        std::string fileContent = Loader::loadFile(resolved_path_str);
        Lexer importLexer(fileContent);
        Parser importParser(importLexer, resolved_path_str);
        importParser.parse();
        std::string finalItemName = alias.empty() ? itemName : alias;
        bool found = false;
        if (importType == "Element") {
            auto* tmpl = importParser.templateManager.getElementTemplate("_global", itemName);
            if (tmpl) {
                parser.templateManager.addElementTemplate(parser.getCurrentNamespace(), finalItemName, NodeCloner::clone(tmpl));
                found = true;
            }
        }
        if (!found) {
             throw std::runtime_error("Could not find item '" + itemName + "' in file '" + path + "'.");
        }
    } else {
        std::string fileContent = Loader::loadFile(resolved_paths[0].string());
        if (alias.empty()) {
            throw std::runtime_error("Raw asset import for @" + importType + " requires an 'as' alias.");
        }
        auto originNode = std::make_unique<OriginNode>(importType, fileContent);
        parser.templateManager.addNamedOrigin(parser.getCurrentNamespace(), alias, std::move(originNode));
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

    const std::string& source = parser.lexer.getSource();
    size_t start_pos = parser.lexer.getPosition();
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

    parser.lexer.setPosition(scan_pos + 1);

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

    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens();
        while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            auto node = this->handle(parser);
        }
        parser.expectToken(TokenType::CloseBrace);
        parser.popNamespace();
    }
}

std::unique_ptr<BaseNode> StatementState::parseScriptElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // script
    parser.expectToken(TokenType::OpenBrace);

    std::stringstream content_ss;
    size_t start_pos = parser.currentToken.start_pos;
    size_t end_pos = start_pos;
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        end_pos = parser.currentToken.start_pos + parser.currentToken.value.length();
        parser.advanceTokens();
    }
    std::string scriptContent = parser.lexer.getSource().substr(start_pos, end_pos - start_pos);

    if (parser.contextNode) {
        std::regex selector_regex(R"(\{\{\s*([.#])([a-zA-Z0-9_-]+)\s*\}\})");
        std::smatch matches;
        std::string::const_iterator search_start(scriptContent.cbegin());

        while (std::regex_search(search_start, scriptContent.cend(), matches, selector_regex)) {
            std::string type = matches[1];
            std::string name = matches[2];

            if (type == "." && parser.contextNode->attributes.find("class") == parser.contextNode->attributes.end()) {
                parser.contextNode->attributes["class"] = std::make_unique<StaticStyleNode>(name);
            } else if (type == "#" && parser.contextNode->attributes.find("id") == parser.contextNode->attributes.end()) {
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