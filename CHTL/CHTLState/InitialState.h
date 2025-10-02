#pragma once

#include "ParserState.h"

namespace CHTL {

    class InitialState : public ParserState {
    public:
        void handleToken(CHTLParserContext* context, const Token& token) override;
    };

}