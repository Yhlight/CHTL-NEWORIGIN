#pragma once

#include "CHTLJSNode.h"
#include <string>

class VirtualObjectAccessNode : public CHTLJSNode {
public:
    VirtualObjectAccessNode(const std::string& objectName, const std::string& propertyName)
        : objectName(objectName), propertyName(propertyName) {}

    CHTLJSNodeType getType() const override {
        return CHTLJSNodeType::VirtualObjectAccess;
    }

    std::string objectName;
    std::string propertyName;
};