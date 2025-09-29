#include "ImportState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ImportNode.h"
#include "StatementState.h"
#include <stdexcept>

// Helper function to map string to ImportCategory
ImportCategory mapStringToImportCategory(const std::string& s, bool isCustom, bool isTemplate, bool isOrigin) {
    if (isCustom) return ImportCategory::Custom;
    if (isTemplate) return ImportCategory::Template;
    if (isOrigin) return ImportCategory::Origin;
    if (s == "Chtl") return ImportCategory::Chtl;
    if (s == "Html") return ImportCategory::Html;
    if (s == "Style") return ImportCategory::Style;
    if (s == "JavaScript") return ImportCategory::JavaScript;
    if (s == "CJmod") return ImportCategory::CJmod;
    if (s == "Config") return ImportCategory::Config;
    // Default if no @type is specified (e.g., [Import] [Template] from ...)
    if (s.empty()) {
        if (isCustom) return ImportCategory::Custom;
        if (isTemplate) return ImportCategory::Template;
        if (isOrigin) return ImportCategory::Origin;
    }
    // Fallback or error
    throw std::runtime_error("Unknown import category type: " + s);
}

// Helper function to map string to ImportType
ImportType mapStringToImportType(const std::string& s) {
    if (s.empty()) return ImportType::None;
    if (s == "Element") return ImportType::Element;
    if (s == "Style") return ImportType::Style;
    if (s == "Var") return ImportType::Var;
    if (s == "Config") return ImportType::Config;
    if (s == "Html") return ImportType::Html;
    if (s == "JavaScript") return ImportType::JavaScript;
    if (s == "Chtl") return ImportType::None; // @Chtl has no specific type
    // Fallback or error
    throw std::runtime_error("Unknown import type: " + s);
}

std::unique_ptr<BaseNode> ImportState::handle(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Import);
    parser.expectToken(TokenType::CloseBracket);

    bool isCustom = false;
    bool isTemplate = false;
    bool isOrigin = false;
    std::string importTypeStr;
    std::string itemName;
    std::string path;
    std::string alias;

    if (parser.currentToken.type == TokenType::OpenBracket) {
        if (parser.peekToken.value == "Custom") isCustom = true;
        else if (parser.peekToken.value == "Template") isTemplate = true;
        else if (parser.peekToken.value == "Origin") isOrigin = true;
        else {
             throw std::runtime_error("Expected 'Custom', 'Template' or 'Origin' after '[' in import statement.");
        }
        parser.advanceTokens(); // consume '['
        parser.advanceTokens(); // consume keyword
        parser.expectToken(TokenType::CloseBracket);
    }

    if (parser.currentToken.type == TokenType::At) {
        parser.advanceTokens(); // consume '@'
        importTypeStr = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    // A wildcard import has 'from' immediately after the type.
    // A precise import has an item name first.
    if (parser.currentToken.type != TokenType::From) {
        itemName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    parser.expectKeyword(TokenType::From, "KEYWORD_FROM", "from");

    // Path can be a string literal or an unquoted literal (identifier)
    if (parser.currentToken.type == TokenType::String || parser.currentToken.type == TokenType::Identifier) {
        path = parser.currentToken.value;
        parser.advanceTokens();
    } else {
        throw std::runtime_error("Import path must be a string or identifier literal.");
    }

    // Remove quotes from string literals
    if (!path.empty() && path.front() == '"' && path.back() == '"') {
        path = path.substr(1, path.size() - 2);
    }

    if (parser.tryExpectKeyword(TokenType::As, "KEYWORD_AS", "as")) {
        alias = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    parser.expectToken(TokenType::Semicolon);

    ImportCategory category = mapStringToImportCategory(importTypeStr, isCustom, isTemplate, isOrigin);
    ImportType type = mapStringToImportType(importTypeStr);

    // After parsing, transition back to the statement state for the next statement.
    parser.setState(std::make_unique<StatementState>());

    // The loader will handle this node later.
    // NOTE: The original implementation processed the import immediately.
    // This is a refactoring to separate parsing from loading.
    return std::make_unique<ImportNode>(category, type, path, itemName, alias);
}