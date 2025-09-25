#pragma once

#include "ParserState.h"

class UseState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};