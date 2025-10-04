#include "ImportParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ImportNode.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> ImportParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Import'
    context->advance(); // consume ']'

    ImportCategory category = ImportCategory::NONE;
    std::string itemType = "";
    std::string itemName = "";
    std::string path = "";
    std::string alias = "";
    ImportType importType = ImportType::UNKNOWN;

    // Check for an optional category like [Custom] or [Template]
    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACKET) {
        context->advance(); // consume '['
        const std::string& categoryName = context->getCurrentToken().lexeme;
        if (categoryName == "Custom") {
            category = ImportCategory::CUSTOM;
        } else if (categoryName == "Template") {
            category = ImportCategory::TEMPLATE;
        } else if (categoryName == "Origin") {
            category = ImportCategory::ORIGIN;
        } else if (categoryName == "Configuration") {
            category = ImportCategory::CONFIGURATION;
        }
        context->advance(); // consume category name
        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) {
            throw std::runtime_error("Expected ']' after import category.");
        }
        context->advance(); // consume ']'
    }

    // The next token is the type, e.g., @Element, @Chtl
    itemType = context->getCurrentToken().lexeme;
    context->advance(); // consume item type

    if (category == ImportCategory::NONE) {
        if (itemType == "@Html") importType = ImportType::HTML;
        else if (itemType == "@Style") importType = ImportType::STYLE;
        else if (itemType == "@JavaScript") importType = ImportType::JAVASCRIPT;
        else if (itemType == "@Chtl") importType = ImportType::CHTL;
        else if (itemType == "@CJmod") importType = ImportType::CJMOD;
        else if (itemType == "@Config") importType = ImportType::CONFIG;
    } else {
        importType = ImportType::CHTL; // Detailed imports are a form of CHTL import
    }

    // The next token might be an item name (for precise imports) or 'from'
    if (context->getCurrentToken().lexeme != "from") {
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
             itemName = context->getCurrentToken().lexeme;
             context->advance(); // consume item name
        }
    }

    if (context->getCurrentToken().lexeme == "from") {
        context->advance(); // consume 'from'
        if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
            path = context->getCurrentToken().lexeme;
            context->advance(); // consume path
        } else {
             throw std::runtime_error("Expected path after 'from' in import statement.");
        }
    } else {
        throw std::runtime_error("Expected 'from' in import statement.");
    }

    if (context->getCurrentToken().lexeme == "as") {
        context->advance(); // consume 'as'
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
            alias = context->getCurrentToken().lexeme;
            context->advance(); // consume alias
        }
    }

    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
        context->advance(); // consume ';'
    }

    if ((importType == ImportType::HTML || importType == ImportType::STYLE) && alias.empty()) {
        throw std::runtime_error("Import for @Html or @Style must have an 'as' alias.");
    }
    if (importType == ImportType::JAVASCRIPT && alias.empty()) {
        return nullptr;
    }

    // Use detailed constructor for all CHTL-related imports to preserve itemType
    if (importType == ImportType::CHTL) {
        return std::make_shared<ImportNode>(category, itemType, itemName, path, alias);
    } else {
        return std::make_shared<ImportNode>(importType, path, alias);
    }
}

}