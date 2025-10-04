#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../CHTLLexer/Token.h"
#include "../CHTLLexer/CHTLLexer.h"

// Forward declarations to avoid circular dependencies
class CHTLParser;
class ExpressionNode;

class ExpressionParser {
public:
    ExpressionParser(const std::string& input);
    ExpressionParser(CHTLParser& parser);
    std::unique_ptr<ExpressionNode> parse();

private:
    std::unique_ptr<ExpressionNode> parseExpression(int precedence);
    std::unique_ptr<ExpressionNode> parsePrefix();
    std::unique_ptr<ExpressionNode> parseInfix(std::unique_ptr<ExpressionNode> left);
    int getPrecedence();
    Token consume();
    Token peek();

    CHTLParser* parser_ptr = nullptr;
    std::unique_ptr<CHTLLexer> ownedLexer;
    std::vector<Token> tokenBuffer;
};