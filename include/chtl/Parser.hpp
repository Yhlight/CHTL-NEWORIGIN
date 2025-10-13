#pragma once
#include "chtl/Lexer.hpp"
#include "chtl/AST.hpp"

namespace chtl {

class Parser {
public:
    explicit Parser(Lexer lexer);

    Document parseDocument();

private:
    Lexer lexer_;
    Token current_{};

    void advance();
    bool accept(TokenKind kind);
    bool expect(TokenKind kind, const char* what);

    std::unique_ptr<Node> parseNode();
    std::unique_ptr<Node> parseText();
    std::unique_ptr<Node> parseComment();
    std::unique_ptr<Node> parseElement();

    std::vector<Attribute> parseAttributes();
};

} // namespace chtl
