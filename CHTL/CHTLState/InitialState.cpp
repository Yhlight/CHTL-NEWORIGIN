#include "InitialState.h"
#include "ElementParsingState.h"
#include "BodyParsingState.h"
#include "../CHTLParser/CHTLParser.h"
#include <stdexcept>
#include <iostream>
#include <memory>

namespace CHTL {

void InitialState::handle(CHTLParser& parser) {
    if (parser.isAtEnd()) {
        parser.setState(nullptr); // End of parsing
        return;
    }

    Token token = parser.peek();

    switch(token.type) {
        case TokenType::IDENTIFIER:
            parser.setState(std::make_unique<ElementParsingState>());
            break;

        case TokenType::R_BRACE:
            parser.consume(TokenType::R_BRACE, "Expected '}'.");
            parser.closeNode();
            parser.setState(std::make_unique<BodyParsingState>());
            break;

        case TokenType::EndOfFile:
            parser.setState(nullptr); // Parsing is done
            break;

        default:
            throw std::runtime_error("Unexpected token at top-level: " + token.value);
    }
}

} // namespace CHTL