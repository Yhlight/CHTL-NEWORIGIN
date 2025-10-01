#include "BodyParsingState.h"
#include "ElementParsingState.h"
#include "InitialState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLStrategy/AttributeStrategy.h"
#include "../CHTLStrategy/CommentStrategy.h"
#include "../CHTLStrategy/ScriptStrategy.h"
#include "../CHTLStrategy/StyleStrategy.h"
#include "../CHTLStrategy/TextNodeStrategy.h"
#include <stdexcept>
#include <iostream>
#include <memory>

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
                    AttributeStrategy strategy;
                    strategy.execute(parser);
                    parser.setState(std::make_unique<BodyParsingState>());
                } else {
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
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::STYLE_KEYWORD:
            parser.consume(TokenType::STYLE_KEYWORD, "Expected 'style' keyword.");
            {
                StyleStrategy strategy;
                strategy.execute(parser);
            }
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::SCRIPT_KEYWORD:
            parser.consume(TokenType::SCRIPT_KEYWORD, "Expected 'script' keyword.");
            {
                ScriptStrategy strategy;
                strategy.execute(parser);
            }
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::GENERATOR_COMMENT:
            {
                CommentStrategy strategy;
                strategy.execute(parser);
            }
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::R_BRACE:
            parser.setState(std::make_unique<InitialState>());
            break;

        case TokenType::EndOfFile:
            parser.setState(nullptr);
            break;

        default:
            throw std::runtime_error("Unexpected token in element body: " + token.value);
    }
}

} // namespace CHTL