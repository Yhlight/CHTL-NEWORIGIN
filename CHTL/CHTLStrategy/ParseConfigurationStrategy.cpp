#include "ParseConfigurationStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

void ParseConfigurationStrategy::Execute(Context* context, Parser& parser) {
    // Consume '[' and 'Configuration'
    parser.ConsumeToken();
    parser.ConsumeToken();

    // Ignore named configs for now: @ConfigName
    if (parser.CurrentToken().type == TokenType::AT) {
        parser.ConsumeToken(); // @
        parser.ConsumeToken(); // ConfigName
    }

    // Expect ]
    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) {
        std::cerr << "Error: Expected ']' after [Configuration." << std::endl;
        return;
    }
    parser.ConsumeToken();

    // Expect {
    if (parser.CurrentToken().type != TokenType::LEFT_BRACE) return;
    parser.ConsumeToken();

    // Parse key-value pairs
    while (parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
        if (parser.CurrentToken().type == TokenType::IDENTIFIER && parser.PeekNextToken().lexeme == "=") {
            std::string key = parser.CurrentToken().lexeme;
            parser.ConsumeToken(); // key
            parser.ConsumeToken(); // =

            std::string value = parser.CurrentToken().lexeme;
            parser.ConsumeToken(); // value

            if (parser.CurrentToken().type == TokenType::SEMICOLON) parser.ConsumeToken();

            // Update configuration object
            if (key == "DEBUG_MODE") {
                context->config.DEBUG_MODE = (value == "true");
            } else if (key == "KEYWORD_TEMPLATE") {
                context->config.KEYWORD_TEMPLATE = value;
            }
            // Add other config keys here...

        } else if (parser.CurrentToken().lexeme == "[Name]") {
            // Skip [Name] block for now
            while(parser.CurrentToken().type != TokenType::RIGHT_BRACE && !parser.IsAtEnd()) {
                parser.ConsumeToken();
            }
        }
        else {
            parser.ConsumeToken(); // Skip unexpected tokens
        }
    }

    if (parser.CurrentToken().type == TokenType::RIGHT_BRACE) parser.ConsumeToken();
}

} // namespace CHTL
