#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLLoader/Loader.h" // Include full definition
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

class Context;

class Parser {
public:
    Parser(Context& context, Loader& loader);
    void Parse(std::vector<Token> tokens);

    const Token& CurrentToken();
    const Token& PeekNextToken();
    const Token& PeekTokenAt(size_t offset) const;
    void ConsumeToken();
    bool IsAtEnd();
    void InjectTokens(const std::vector<Token>& newTokens);

    // Making this public so strategies can access it to handle imports
    Loader& loader;

private:
    Context& context;
    std::vector<Token> tokens;
    size_t currentTokenIndex = 0;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
