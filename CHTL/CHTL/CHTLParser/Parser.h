#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer);

    std::unique_ptr<ProgramNode> parseProgram();
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    void nextToken();
    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);
    void peekError(TokenType type);

    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();

    Lexer& lexer;
    Token currentToken;
    Token peekToken;

    std::vector<std::string> errors;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
