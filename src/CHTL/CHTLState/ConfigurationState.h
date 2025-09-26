#pragma once

#include "ParserState.h"
#include "../CHTLManage/ConfigurationManager.h" // For ConfigSet

// State for parsing the contents of a '[Configuration] { ... }' block.
class ConfigurationState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    // Helper to parse the nested [Name] block for keyword re-definitions.
    void parseNameBlock(Parser& parser, ConfigSet& configSet);
    // Helper to parse the nested [OriginType] block for whitelisting types.
    void parseOriginTypeBlock(Parser& parser, ConfigSet& configSet);
};