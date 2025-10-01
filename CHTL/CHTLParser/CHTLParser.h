#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/ElementNode.h"
#include <memory>

class CHTLParser {
public:
    explicit CHTLParser(const std::string& input);
    std::unique_ptr<ElementNode> parse();

private:
    CHTLLexer lexer;
    Token currentToken;

    void advance();
    void expect(TokenType type);
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<BaseNode> parseTextNode();
    std::unique_ptr<BaseNode> parseCommentNode();
};

#endif //CHTL_PARSER_H