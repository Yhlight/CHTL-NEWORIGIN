#pragma once
#include "chtl/Common.hpp"

namespace chtl {

class Lexer {
public:
    explicit Lexer(std::string_view input);

    Token next();
    const Token& peek();
    bool isAtEnd() const;

private:
    std::string source_;
    size_t index_ {0};
    SourceLocation loc_ {1, 1};
    Token lookahead_{};
    bool hasLookahead_ {false};

    char current() const;
    char advance();
    bool match(char expected);
    void skipWhitespaceAndComments();

    Token make(TokenKind kind, size_t startIndex, SourceLocation startLoc);
    Token lexIdentifier(size_t startIndex, SourceLocation startLoc);
    Token lexString(size_t startIndex, SourceLocation startLoc);
    Token lexNumber(size_t startIndex, SourceLocation startLoc);
    Token lexTextUntil(char terminal);
};

} // namespace chtl
