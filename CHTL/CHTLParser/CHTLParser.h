#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

class ElementNode; // Forward declaration

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens);
    ~CHTLParser();

    std::unique_ptr<BaseNode> parse();

private:
    // Statement parsing
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<BaseNode> parseTextStatement();
    std::unique_ptr<BaseNode> parseElementStatement();
    void parseAttributeStatement(ElementNode& owner);

    // Token stream management
    std::vector<Token> tokens;
    size_t current = 0;

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(std::vector<TokenType> types);
};

} // namespace CHTL

#endif // CHTL_PARSER_H