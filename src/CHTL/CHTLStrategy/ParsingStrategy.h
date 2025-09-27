#pragma once

#include <memory>
#include "CHTL/CHTLNode/BaseNode.h"

class Parser; // Forward declaration to avoid circular dependencies

class ParsingStrategy {
public:
    virtual ~ParsingStrategy() = default;
    virtual std::unique_ptr<BaseNode> parse(Parser& parser) = 0;
};