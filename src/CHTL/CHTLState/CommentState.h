#pragma once

#include "CHTLState.h"

class CommentState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};