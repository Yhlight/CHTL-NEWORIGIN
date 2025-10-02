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
    Token nextToken;

    void advance();
    Token peek();
    void expect(TokenType type);
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    void parseAttribute(ElementNode& node);
    std::unique_ptr<BaseNode> parseTextNode();
    std::unique_ptr<BaseNode> parseCommentNode();
    std::unique_ptr<BaseNode> parseStyleNode();
    std::unique_ptr<BaseNode> parseTextAttribute();
};

#endif //CHTL_PARSER_H