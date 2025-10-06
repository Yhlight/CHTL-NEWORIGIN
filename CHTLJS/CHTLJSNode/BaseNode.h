#pragma once

#include <vector>
#include <memory>

namespace CHTLJS {

enum class NodeType {
    NODE_SELECTOR,
    NODE_STRING_LITERAL,
    // Other node types will be added here
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    NodeType getType() const { return type; }

protected:
    NodeType type;
};

}