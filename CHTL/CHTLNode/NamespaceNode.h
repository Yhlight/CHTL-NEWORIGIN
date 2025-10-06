#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name) : BaseNode(NodeType::NODE_NAMESPACE), name(name) {}

    const std::string& getName() const { return name; }

private:
    std::string name;
};

}