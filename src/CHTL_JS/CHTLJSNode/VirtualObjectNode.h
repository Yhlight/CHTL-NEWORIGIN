#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <memory>

class VirtualObjectNode : public CHTLJSNode {
public:
    VirtualObjectNode(const std::string& name, std::unique_ptr<CHTLJSNode> value)
        : name(name), value(std::move(value)) {}

    CHTLJSNodeType getType() const override {
        return CHTLJSNodeType::VirtualObject;
    }

    std::string name;
    std::unique_ptr<CHTLJSNode> value;
};