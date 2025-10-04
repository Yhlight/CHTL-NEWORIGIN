#include "OriginState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/OriginNode.h"

OriginState::OriginState() : phase(Phase::EXPECT_AT) {}

void OriginState::handle(CHTLParser& parser, Token token) {
    switch (phase) {
        case Phase::EXPECT_AT:
            if (token.type == TokenType::AT_SIGN) {
                phase = Phase::EXPECT_TYPE;
            }
            // Note: In a more robust parser, you might want to handle an error here.
            // For now, we'll just wait for the '@' symbol.
            break;

        case Phase::EXPECT_TYPE:
            if (token.type == TokenType::IDENTIFIER) {
                type = "@" + token.value;
                phase = Phase::EXPECT_NAME_OR_BRACE;
            } else {
                // Syntax error, revert to default state.
                parser.setState(std::make_unique<DefaultState>());
                parser.putback(token);
            }
            break;

        case Phase::EXPECT_NAME_OR_BRACE:
            if (token.type == TokenType::IDENTIFIER) {
                name = token.value;
                phase = Phase::EXPECT_BRACE;
            } else if (token.type == TokenType::LEFT_BRACE) {
                parser.setRawContentMode(true);
                parser.putback(token);
                phase = Phase::EXPECT_CONTENT;
            } else {
                // Syntax error, revert to default state.
                parser.setState(std::make_unique<DefaultState>());
                parser.putback(token);
            }
            break;

        case Phase::EXPECT_BRACE:
            if (token.type == TokenType::LEFT_BRACE) {
                parser.setRawContentMode(true);
                parser.putback(token);
                phase = Phase::EXPECT_CONTENT;
            } else {
                // Syntax error, revert to default state.
                parser.setState(std::make_unique<DefaultState>());
                parser.putback(token);
            }
            break;

        case Phase::EXPECT_CONTENT:
            // After setting raw mode and putting back the '{', the lexer will
            // consume the entire block and return it as a single string literal.
            if (token.type == TokenType::STRING_LITERAL) {
                auto node = std::make_unique<OriginNode>(type, token.value, name);
                parser.addNode(std::move(node));
                parser.setState(std::make_unique<DefaultState>());
            } else {
                // This case would indicate a bug in the lexer or an empty block
                // that wasn't handled as an empty string. Revert to be safe.
                parser.setState(std::make_unique<DefaultState>());
            }
            break;
    }
}