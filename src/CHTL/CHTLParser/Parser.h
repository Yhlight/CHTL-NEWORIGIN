#ifndef PARSER_H
#define PARSER_H

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLNode/Node.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include <vector>
#include <string>
#include <memory>
#include <optional>

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
    std::optional<Attribute> parseAttribute();
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
