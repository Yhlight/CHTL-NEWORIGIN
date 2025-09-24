#include "ParseCustomStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/TemplateNode.h"
#include <iostream>
#include <sstream>

namespace CHTL {

void ParseCustomStrategy::Execute(Context* context, Parser& parser) {
    // Consume '[' and the configured keyword for [Custom]
    parser.ConsumeToken();
    std::string custom_keyword = context->config.KEYWORD_CUSTOM;
    custom_keyword = custom_keyword.substr(1, custom_keyword.length() - 2);
    if (parser.CurrentToken().lexeme != custom_keyword) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::AT) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().lexeme != "Style") {
        // For now, only support @Style customization
        return;
    }
    parser.ConsumeToken(); // Style

    if (parser.CurrentToken().type != TokenType::IDENTIFIER) return;
    std::string templateName = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    CustomStyleGroup customGroup(templateName);

    if (parser.CurrentToken().type != TokenType::LEFT_BRACE) return;
    parser.ConsumeToken();

    // Parse properties
    while (parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
        if (parser.CurrentToken().type == TokenType::IDENTIFIER) {
            std::string propName = parser.CurrentToken().lexeme;
            parser.ConsumeToken();

            if (parser.CurrentToken().type == TokenType::COLON) {
                // Property with value
                parser.ConsumeToken(); // :
                std::stringstream valueBuilder;
                 while(parser.CurrentToken().type != TokenType::SEMICOLON && parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
                    valueBuilder << parser.CurrentToken().lexeme;
                    parser.ConsumeToken();
                }
                customGroup.AddProperty({propName, valueBuilder.str()});
            } else {
                // Valueless property
                customGroup.AddProperty({propName, ""});
            }

            if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();
        } else {
            parser.ConsumeToken();
        }
    }

    if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) parser.ConsumeToken();

    context->namespaces[context->currentNamespace].customStyleTemplates.emplace(templateName, customGroup);
}

} // namespace CHTL