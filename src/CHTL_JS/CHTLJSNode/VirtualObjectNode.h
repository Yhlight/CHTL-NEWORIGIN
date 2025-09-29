#pragma once

#include "CHTLJSBaseNode.h"
#include <string>
#include <memory>

class VirtualObjectNode : public CHTLJSBaseNode {
private:
    std::string name;
    std::unique_ptr<CHTLJSBaseNode> assigned_block;

public:
    VirtualObjectNode(const std::string& obj_name, std::unique_ptr<CHTLJSBaseNode> block)
        : name(obj_name), assigned_block(std::move(block)) {}

    const std::string& getName() const {
        return name;
    }

    const CHTLJSBaseNode* getAssignedBlock() const {
        return assigned_block.get();
    }

    virtual std::string toString() const override {
        return "VirtualObjectNode(Name: " + name + ", Block: " + (assigned_block ? assigned_block->toString() : "null") + ")";
    }
};