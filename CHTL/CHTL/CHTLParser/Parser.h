#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<BaseNode> parse();

private:
    std::unique_ptr<ElementNode> parseElement();
    void parseBlock(ElementNode* element);
    void parseAttribute(ElementNode* element);
    std::unique_ptr<TextNode> parseTextNode();


    bool isAtEnd();
    Token advance();
    Token peek();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();

    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace CHTL
