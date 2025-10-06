#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

    enum class NodeType {
        NODE_SCRIPT_LOADER
    };

    class BaseNode {
    public:
        BaseNode(NodeType type) : type(type) {}
        virtual ~BaseNode() = default;
        NodeType getType() const { return type; }

    private:
        NodeType type;
    };

}