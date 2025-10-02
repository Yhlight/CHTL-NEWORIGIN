#ifndef CHTL_DOCUMENT_NODE_H
#define CHTL_DOCUMENT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>
#include <sstream>

class DocumentNode : public BaseNode {
public:
    DocumentNode() = default;

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "[Document]" << std::endl;
        for (const auto& child : children) {
            ss << child->toString(depth + 1);
        }
        return ss.str();
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

private:
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif //CHTL_DOCUMENT_NODE_H