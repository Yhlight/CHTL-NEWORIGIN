#pragma once

#include "CHTLJSToken.h"
#include <string>
#include <vector>

namespace CHTL_JS {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(std::string source);

    std::vector<CHTLJSToken> getAllTokens();

private:
    CHTLJSToken getNextToken();
    void advance(int count = 1);
    char currentChar() const;
    char peekChar() const;

    CHTLJSToken makeToken(CHTLJSTokenType type, const std::string& value);
    CHTLJSToken identifier();
    std::string parseFunctionBody();

    std::string m_source;
    size_t m_position = 0;
};

} // namespace CHTL_JS
