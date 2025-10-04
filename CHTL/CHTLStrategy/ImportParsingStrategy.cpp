#include "ImportParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ImportNode.h"

namespace CHTL {

std::shared_ptr<BaseNode> ImportParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Import'
    context->advance(); // consume ']'

    ImportType importType = ImportType::UNKNOWN;
    if (context->getCurrentToken().lexeme == "@Html") {
        importType = ImportType::HTML;
    } else if (context->getCurrentToken().lexeme == "@Style") {
        importType = ImportType::STYLE;
    } else if (context->getCurrentToken().lexeme == "@JavaScript") {
        importType = ImportType::JAVASCRIPT;
    } else if (context->getCurrentToken().lexeme == "@Chtl") {
        importType = ImportType::CHTL;
    } else if (context->getCurrentToken().lexeme == "@CJmod") {
        importType = ImportType::CJMOD;
    } else if (context->getCurrentToken().lexeme == "@Config") {
        importType = ImportType::CONFIG;
    }
    context->advance(); // consume import type

    std::string path = "";
    if (context->getCurrentToken().lexeme == "from") {
        context->advance(); // consume 'from'
        if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
            path = context->getCurrentToken().lexeme;
            context->advance(); // consume path
        }
    }

    std::string alias = "";
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

    return std::make_shared<ImportNode>(importType, path, alias);
}

}