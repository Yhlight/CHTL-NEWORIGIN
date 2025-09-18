#pragma once

#include "CHTLJS_BaseNode.h"
#include <map>

namespace CHTL_JS {

// Represents a Listen { ... } block.
class ListenNode : public CHTLJS_BaseNode {
public:
    ListenNode(std::unique_ptr<CHTLJS_BaseNode> target, std::map<std::string, std::string> events)
        : target(std::move(target)), events(std::move(events)) {}

    void accept(CHTLJS_AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<CHTLJS_BaseNode> target;
    std::map<std::string, std::string> events;
};

} // namespace CHTL_JS
