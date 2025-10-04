#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name) : name(name) {
        type = NodeType::NODE_NAMESPACE;
    }

    const std::string& getName() const { return name; }

private:
    std::string name;
};

}