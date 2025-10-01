#include "BodyParsingState.h"
#include "ElementParsingState.h"
#include "InitialState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLStrategy/AttributeStrategy.h"
#include "../CHTLStrategy/StyleStrategy.h"
#include "../CHTLStrategy/TextNodeStrategy.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

void BodyParsingState::handle(CHTLParser& parser) {
    if (parser.isAtEnd()) {
        throw std::runtime_error("Reached end of file unexpectedly while parsing element body.");
    }

    Token token = parser.peek();

    switch(token.type) {
        case TokenType::IDENTIFIER:
            {
                Token nextToken = parser.peekNext();
                if (nextToken.type == TokenType::COLON || nextToken.type == TokenType::EQUAL) {
                    // It's an attribute
                    AttributeStrategy strategy;
                    strategy.execute(parser);
                    // Stay in the body state to parse more content
                    parser.setState(std::make_unique<BodyParsingState>());
                } else {
                    // It's a nested element
                    parser.setState(std::make_unique<ElementParsingState>());
                }
            }
            break;

        case TokenType::TEXT_KEYWORD:
            parser.consume(TokenType::TEXT_KEYWORD, "Expected 'text' keyword.");
            {
                TextNodeStrategy strategy;
                strategy.execute(parser);
            }
            // After parsing a text node, we remain in the body of the current element.
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::STYLE_KEYWORD:
            parser.consume(TokenType::STYLE_KEYWORD, "Expected 'style' keyword.");
            {
                StyleStrategy strategy;
                strategy.execute(parser);
            }
            // After parsing a style block, we remain in the body of the current element.
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::R_BRACE:
            // The end of the current element is handled by the InitialState.
            parser.setState(std::make_unique<InitialState>());
            break;

        case TokenType::EndOfFile:
            parser.setState(nullptr); // Parsing is done
            break;

        default:
            throw std::runtime_error("Unexpected token in element body: " + token.value);
    }
}

} // namespace CHTL