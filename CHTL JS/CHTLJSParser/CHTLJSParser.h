#pragma once

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL_JS {

class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<CHTLJSToken> tokens);

    std::vector<std::unique_ptr<CHTLJS_BaseNode>> parse();

private:
    const CHTLJSToken& currentToken() const;
    void advance();

    std::vector<CHTLJSToken> m_tokens;
    size_t m_position = 0;
};

} // namespace CHTL_JS
