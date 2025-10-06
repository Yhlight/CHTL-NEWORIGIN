#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>

namespace CHTL {
    class CHTLParserContext;

    class ParserState {
    public:
        virtual ~ParserState() = default;
        virtual void handleToken(CHTLParserContext* context, const Token& token) = 0;
    };
}