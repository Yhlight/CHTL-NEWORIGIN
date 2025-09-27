#pragma once

#include "ParserState.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <memory>

class NamespaceState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};