#pragma once

#include "ParserState.h"

class InfoState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};