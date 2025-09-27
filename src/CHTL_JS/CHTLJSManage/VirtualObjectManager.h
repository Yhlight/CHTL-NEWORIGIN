#pragma once

#include "../CHTLJSNode/VirtualObjectNode.h"
#include <string>
#include <memory>
#include <unordered_map>

class VirtualObjectManager {
public:
    void registerVirtualObject(const std::string& name, std::unique_ptr<VirtualObjectNode> node);
    VirtualObjectNode* getVirtualObject(const std::string& name) const;
    void clear();

private:
    std::unordered_map<std::string, std::unique_ptr<VirtualObjectNode>> virtualObjects;
};