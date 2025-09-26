#pragma once

#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include <vector>
#include <memory>

// The CHTL JS Parser, responsible for building an AST from a token stream.
class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<CHTLJSToken> tokens);
    std::vector<std::unique_ptr<CHTLJSNode>> parse();

private:
    std::vector<CHTLJSToken> tokens;
    size_t position = 0;

    CHTLJSToken currentToken() const;
    void advance();
};