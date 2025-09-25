#include "StyleParsingUtil.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLState/StyleBlockState.h" // For parseStyleExpression
#include "StyleUtil.h" // For styleValueToString
#include <stdexcept>

std::string parseCssRuleBlock(Parser& parser) {
    parser.expectToken(TokenType::OpenBrace);
    std::string cssRules;
    while(parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected property name inside selector block.");
        std::string key = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Colon);

        // We need an instance of StyleBlockState to call parseStyleExpression
        StyleBlockState tempState;
        StyleValue sv = tempState.parseStyleExpression(parser);

        std::string value = styleValueToString(sv);
        cssRules += "  " + key + ": " + value + ";\n";
        if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
    }
    parser.expectToken(TokenType::CloseBrace);
    return cssRules;
}