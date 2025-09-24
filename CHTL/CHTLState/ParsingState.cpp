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
            const std::string& nextLexeme = parser.PeekNextToken().lexeme;
            if (nextLexeme == "Import") {
                this->parseImportStrategy->Execute(context, parser);
            } else if (nextLexeme == "Template") {
                this->parseTemplateStrategy->Execute(context, parser);
            } else if (nextLexeme == "Namespace") {
                this->parseNamespaceStrategy->Execute(context, parser);
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
