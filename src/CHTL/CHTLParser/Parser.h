#ifndef PARSER_H
#define PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<Program> ParseProgram();
    const std::vector<std::string>& Errors() const { return errors; }

private:
    void nextToken();
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    void parseBlock(ElementNode* element);

    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);

    void peekError(TokenType type);

    Lexer& lexer;
    Token currentToken;
    Token peekToken;
    std::vector<std::string> errors;
};

} // namespace CHTL

#endif // PARSER_H
