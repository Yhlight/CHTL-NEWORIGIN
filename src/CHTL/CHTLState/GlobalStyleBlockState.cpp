#include "GlobalStyleBlockState.h"
#include "../CHTLParser/Parser.h"
#include <stdexcept>
#include <sstream>

std::unique_ptr<BaseNode> GlobalStyleBlockState::handle(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // consume 'style'
    parser.expectToken(TokenType::OpenBrace);

    std::stringstream cssContent;
    int braceLevel = 1;

    // A simple, non-parsing loop to capture the raw content of the style block.
    // This avoids the null-pointer crash by not using the expression parser.
    while (braceLevel > 0 && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::OpenBrace) {
            braceLevel++;
        } else if (parser.currentToken.type == TokenType::CloseBrace) {
            braceLevel--;
            if (braceLevel == 0) {
                break; // End of block
            }
        }
        cssContent << parser.currentToken.value << " ";
        parser.advanceTokens();
    }

    parser.globalStyleContent += cssContent.str();

    parser.expectToken(TokenType::CloseBrace);
    return nullptr;
}