#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <memory>

class CHTLParser {
public:
    CHTLParser(const std::string& input, CHTLContext& context);
    std::unique_ptr<DocumentNode> parse();

private:
    CHTLLexer lexer;
    CHTLContext& context;
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
    std::unique_ptr<BaseNode> parseTemplateDeclaration();
};

#endif //CHTL_PARSER_H