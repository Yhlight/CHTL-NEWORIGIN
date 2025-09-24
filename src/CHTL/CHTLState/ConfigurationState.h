#pragma once

#include "ParserState.h"

// State for parsing the contents of a '[Configuration] { ... }' block.
class ConfigurationState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    // Helper to parse the nested [Name] block for keyword re-definitions.
    void parseNameBlock(Parser& parser);
};
