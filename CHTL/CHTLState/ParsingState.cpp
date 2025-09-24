#include "ParsingState.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

ParsingState::ParsingState() {
    this->parseElementStrategy = std::make_unique<ParseElementStrategy>();
    this->parseTemplateStrategy = std::make_unique<ParseTemplateStrategy>();
    this->parseImportStrategy = std::make_unique<ParseImportStrategy>();
    this->parseNamespaceStrategy = std::make_unique<ParseNamespaceStrategy>();
    this->parseConfigurationStrategy = std::make_unique<ParseConfigurationStrategy>();
    this->parseCustomStrategy = std::make_unique<ParseCustomStrategy>();
}

void ParsingState::Handle(Context* context, Parser& parser) {
    if (parser.IsAtEnd()) {
        return;
    }

    switch (parser.CurrentToken().type) {
        case TokenType::IDENTIFIER:
            this->parseElementStrategy->Execute(context, parser);
            break;

        case TokenType::LEFT_BRACKET: {
            // Remove brackets from keyword config strings for comparison
            std::string import_keyword = context->config.KEYWORD_IMPORT.substr(1, context->config.KEYWORD_IMPORT.length() - 2);
            std::string template_keyword = context->config.KEYWORD_TEMPLATE.substr(1, context->config.KEYWORD_TEMPLATE.length() - 2);
            std::string namespace_keyword = context->config.KEYWORD_NAMESPACE.substr(1, context->config.KEYWORD_NAMESPACE.length() - 2);
            std::string custom_keyword = context->config.KEYWORD_CUSTOM.substr(1, context->config.KEYWORD_CUSTOM.length() - 2);

            const std::string& nextLexeme = parser.PeekNextToken().lexeme;

            if (nextLexeme == import_keyword) {
                this->parseImportStrategy->Execute(context, parser);
            } else if (nextLexeme == template_keyword) {
                this->parseTemplateStrategy->Execute(context, parser);
            } else if (nextLexeme == namespace_keyword) {
                this->parseNamespaceStrategy->Execute(context, parser);
            } else if (nextLexeme == custom_keyword) {
                this->parseCustomStrategy->Execute(context, parser);
            }
            else if (nextLexeme == "Configuration") { // This one is not configurable
                this->parseConfigurationStrategy->Execute(context, parser);
            }
            else {
                std::cerr << "Warning: Unexpected token '" << nextLexeme << "' after '[', skipping." << std::endl;
                parser.ConsumeToken(); // Consume the [
            }
            break;
        }

        case TokenType::RIGHT_BRACE:
            std::cerr << "Warning: Stray '}' found, skipping." << std::endl;
            parser.ConsumeToken();
            break;

        default:
            std::cerr << "Warning: Unexpected token " << TokenTypeToString(parser.CurrentToken().type) << " at top level, skipping." << std::endl;
            parser.ConsumeToken();
            break;
    }
}

} // namespace CHTL
