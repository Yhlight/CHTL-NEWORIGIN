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

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACKET) {
        context->advance(); // consume '['
        const std::string& categoryName = context->getCurrentToken().lexeme;
        if (categoryName == "Custom") category = ImportCategory::CUSTOM;
        else if (categoryName == "Template") category = ImportCategory::TEMPLATE;
        else if (categoryName == "Origin") category = ImportCategory::ORIGIN;
        else if (categoryName == "Configuration") category = ImportCategory::CONFIGURATION;
        context->advance(); // consume category name
        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) throw std::runtime_error("Expected ']' after import category.");
        context->advance(); // consume ']'
    }

    if (context->getCurrentToken().type == TokenType::TOKEN_AT) {
        itemType += context->getCurrentToken().lexeme; // Consume '@'
        context->advance();
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
            itemType += context->getCurrentToken().lexeme; // Consume type, e.g., 'Chtl'
            context->advance();
        } else {
            throw std::runtime_error("Expected identifier after '@' in import statement.");
        }
    }

    if (category == ImportCategory::NONE) {
        if (itemType == "@Html") importType = ImportType::HTML;
        else if (itemType == "@Style") importType = ImportType::STYLE;
        else if (itemType == "@JavaScript") importType = ImportType::JAVASCRIPT;
        else if (itemType == "@Chtl") importType = ImportType::CHTL;
        else if (itemType == "@CJmod") importType = ImportType::CJMOD;
        else if (itemType == "@Config") importType = ImportType::CONFIG;
    } else {
        importType = ImportType::CHTL;
    }

    if (context->getCurrentToken().type != TokenType::TOKEN_KEYWORD_FROM) {
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
             itemName = context->getCurrentToken().lexeme;
             context->advance();
        }
    }

    if (context->getCurrentToken().type == TokenType::TOKEN_KEYWORD_FROM) {
        context->advance();
        if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
            path = context->getCurrentToken().lexeme;
            context->advance();
        } else {
             throw std::runtime_error("Expected path after 'from' in import statement.");
        }
    } else {
        throw std::runtime_error("Expected 'from' in import statement.");
    }

    if (context->getCurrentToken().lexeme == "as") {
        context->advance();
        if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
            alias = context->getCurrentToken().lexeme;
            context->advance();
        }
    }

    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
        context->advance();
    }

    if ((importType == ImportType::HTML || importType == ImportType::STYLE) && alias.empty()) {
        throw std::runtime_error("Import for @Html or @Style must have an 'as' alias.");
    }
    if (importType == ImportType::JAVASCRIPT && alias.empty()) {
        return nullptr;
    }

    if (importType == ImportType::CHTL) {
        return std::make_shared<ImportNode>(category, itemType, itemName, path, alias);
    } else {
        return std::make_shared<ImportNode>(importType, path, alias);
    }
}

}