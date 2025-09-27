#include "VirtualObjectManager.h"

void VirtualObjectManager::registerVirtualObject(const std::string& name, std::unique_ptr<VirtualObjectNode> node) {
    virtualObjects[name] = std::move(node);
}

VirtualObjectNode* VirtualObjectManager::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects.find(name);
    if (it != virtualObjects.end()) {
        return it->second.get();
    }
    return nullptr;
}