#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "BaseExprNode.h"
#include <memory>

// Forward declare CHTLParser to avoid circular include
class CHTLParser;
class CHTLContext;

class ExprParser {
public:
    explicit ExprParser(CHTLParser& chtlParser, CHTLContext& context);
    std::shared_ptr<BaseExprNode> parse();

private:
    CHTLParser& parser; // Reference to the main parser to access tokens
    CHTLContext& context;

    // Recursive descent methods based on operator precedence
    std::shared_ptr<BaseExprNode> parseExpression();
    std::shared_ptr<BaseExprNode> parseTernary();
    std::shared_ptr<BaseExprNode> parseLogicalOr();
    std::shared_ptr<BaseExprNode> parseLogicalAnd();
    std::shared_ptr<BaseExprNode> parseEquality();
    std::shared_ptr<BaseExprNode> parseComparison();
    std::shared_ptr<BaseExprNode> parseTerm();
    std::shared_ptr<BaseExprNode> parseFactor();
    std::shared_ptr<BaseExprNode> parsePower();
    std::shared_ptr<BaseExprNode> parsePrimary();

    // Helper to get precedence of a token
    int getPrecedence(TokenType type);
};
