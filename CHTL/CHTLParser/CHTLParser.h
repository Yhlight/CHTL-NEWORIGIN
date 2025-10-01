#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/ExpressionNode.h"

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<BaseNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Token manipulation helpers
    Token peek() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // Statement-level parsing
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<BaseNode> parseTextNode();
    std::unique_ptr<StyleNode> parseStyleNode(ElementNode* parent);
    std::unique_ptr<ScriptNode> parseScriptNode();
    void parseAttribute(ElementNode* element);

    // Expression parsing (Pratt/Precedence Climbing)
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<ExpressionNode> parseTerm();      // Addition and Subtraction
    std::unique_ptr<ExpressionNode> parseFactor();    // Multiplication, Division, Modulo
    std::unique_ptr<ExpressionNode> parsePower();     // Exponentiation (right-associative)
    std::unique_ptr<ExpressionNode> parsePrimary();   // Literals and grouping
};

} // namespace CHTL

#endif // CHTL_PARSER_H