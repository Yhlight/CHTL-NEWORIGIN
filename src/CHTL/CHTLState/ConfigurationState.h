#pragma once

#include "ParserState.h"
#include "../CHTLManage/ConfigurationManager.h" // For ConfigSet
#include "../CHTLStrategy/ConfigSetterStrategy.h"
#include <memory>
#include <string>
#include <unordered_map>

// State for parsing the contents of a '[Configuration] { ... }' block.
class ConfigurationState : public ParserState {
public:
    ConfigurationState();
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    // Helper to parse the nested [Name] block for keyword re-definitions.
    void parseNameBlock(Parser& parser, ConfigSet& configSet);

    // Map to hold the strategies for setting configuration values.
    std::unordered_map<std::string, std::unique_ptr<IConfigSetter>> configSetters;
};