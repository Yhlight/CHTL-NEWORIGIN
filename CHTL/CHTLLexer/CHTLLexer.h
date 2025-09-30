#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include "Token.h"

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();

    std::vector<Token> tokenize(const std::string& input);
};

} // namespace CHTL

#endif // CHTL_LEXER_H