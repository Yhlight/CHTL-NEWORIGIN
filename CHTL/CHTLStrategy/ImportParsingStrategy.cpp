#include "ImportParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ImportNode.h"
#include <stdexcept>

namespace CHTL {

// Helper to map the parsed type string to the ImportType enum
ImportType stringToImportType(const std::string& s) {
    if (s == "@Html") return ImportType::HTML;
    if (s == "@Style") return ImportType::STYLE;
    if (s == "@JavaScript") return ImportType::JAVASCRIPT;
    if (s == "@Chtl") return ImportType::CHTL;
    if (s == "@CJmod") return ImportType::CJMOD;
    if (s == "@Config") return ImportType::CONFIG;
    // For specific types like @Element, @Var, etc., the overarching type is CHTL
    return ImportType::CHTL;
}

std::shared_ptr<BaseNode> ImportParsingStrategy::parse(CHTLParserContext* context) {
    // This strategy is invoked after the parser has seen and consumed '[Import]'.
    // The current token is what comes *after* '[Import]'.

    ImportCategory category = ImportCategory::NONE;
    std::string itemTypeStr;
    std::string itemName;
    std::string path;
    std::string alias;

    // 1. Parse optional category: e.g., [Custom], [Template]
    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACKET) {
        context->advance(); // consume '['
        const std::string& categoryName = context->getCurrentToken().lexeme;
        if (categoryName == "Custom") category = ImportCategory::CUSTOM;
        else if (categoryName == "Template") category = ImportCategory::TEMPLATE;
        else if (categoryName == "Origin") category = ImportCategory::ORIGIN;
        else if (categoryName == "Configuration") category = ImportCategory::CONFIGURATION;
        else throw std::runtime_error("Unknown import category: " + categoryName);
        context->advance(); // consume category name
        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) throw std::runtime_error("Expected ']' after import category.");
        context->advance(); // consume ']'
    }

    // 2. Parse the import type: e.g., @Element, @Chtl
    if (context->getCurrentToken().type != TokenType::TOKEN_AT) {
        throw std::runtime_error("Expected '@' to specify import type.");
    }
    itemTypeStr += context->getCurrentToken().lexeme; // '@'
    context->advance();
    if (context->getCurrentToken().type != TokenType::TOKEN_IDENTIFIER) {
        throw std::runtime_error("Expected import type identifier after '@'.");
    }
    itemTypeStr += context->getCurrentToken().lexeme;
    context->advance();

    // 3. Parse optional item name (for precise imports like `... @Element MyElement from ...`)
    if (context->getCurrentToken().type != TokenType::TOKEN_KEYWORD_FROM) {
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
            itemName = context->getCurrentToken().lexeme;
            context->advance();
        } else {
            throw std::runtime_error("Unexpected token '" + context->getCurrentToken().lexeme + "' before 'from' keyword.");
        }
    }

    // 4. Parse 'from' and the file path
    if (context->getCurrentToken().type != TokenType::TOKEN_KEYWORD_FROM) {
        throw std::runtime_error("Expected 'from' keyword in import statement.");
    }
    context->advance(); // consume 'from'
    if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
        path = context->getCurrentToken().lexeme;
        context->advance();
    } else {
        throw std::runtime_error("Expected a path string or literal after 'from'.");
    }

    // 5. Parse optional 'as' alias
    if (context->getCurrentToken().lexeme == "as") {
        context->advance(); // consume 'as'
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
            alias = context->getCurrentToken().lexeme;
            context->advance();
        } else {
            throw std::runtime_error("Expected an alias identifier after 'as'.");
        }
    }

    // 6. Consume optional semicolon
    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
        context->advance();
    }

    // 7. Validation and Node Creation
    ImportType importType = stringToImportType(itemTypeStr);

    if ((importType == ImportType::HTML || importType == ImportType::STYLE) && alias.empty()) {
        throw std::runtime_error("Import for @Html or @Style must have an 'as' alias.");
    }

    if (importType == ImportType::JAVASCRIPT && alias.empty()) {
        // As per spec, this is valid and results in no action.
        return nullptr;
    }

    if (category != ImportCategory::NONE) {
        return std::make_shared<ImportNode>(category, itemTypeStr, itemName, path, alias);
    } else {
        return std::make_shared<ImportNode>(importType, path, alias);
    }
}

}