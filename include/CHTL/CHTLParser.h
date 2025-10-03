#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;

    std::unique_ptr<BaseNode> parse(const std::vector<Token>& tokens);
};

} // namespace CHTL