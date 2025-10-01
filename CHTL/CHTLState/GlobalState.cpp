#include "GlobalState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLStrategy/ElementStrategy.h"
#include "../CHTLStrategy/TextNodeStrategy.h"
#include "../CHTLStrategy/TemplateDefinitionStrategy.h"
#include "../CHTLStrategy/TemplateUsageStrategy.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

void GlobalState::handle(CHTLParser& parser) {
    while (parser.match(TokenType::COMMENT)) {}
    if(parser.isAtEnd()) return;

    if (parser.check(TokenType::L_BRACKET)) {
        TemplateDefinitionStrategy strategy;
        parser.addNodeToCurrent(strategy.parse(parser));
    }
    else if (parser.check(TokenType::IDENTIFIER)) {
        ElementStrategy strategy;
        parser.addNodeToCurrent(strategy.parse(parser));
    }
    else if (parser.match(TokenType::TEXT_KEYWORD)) {
        TextNodeStrategy strategy;
        parser.addNodeToCurrent(strategy.parse(parser));
    }
    else if (parser.match(TokenType::AT_SIGN)) {
        TemplateUsageStrategy strategy;
        parser.addNodeToCurrent(strategy.parse(parser));
    }
    else {
        // If no strategy matches, advance to avoid infinite loops on unknown tokens
        if (!parser.isAtEnd()) {
            parser.advance();
        }
    }
}

} // namespace CHTL