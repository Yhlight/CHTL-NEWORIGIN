#pragma once

#include "ParserState.h"

// The ImportState is responsible for parsing [Import] statements.
// It determines the type of import and extracts the necessary information,
// such as the path and any aliases.
class ImportState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};