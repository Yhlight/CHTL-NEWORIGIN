#pragma once

#include <memory>
#include <vector>

class BaseNode {
public:
    virtual ~BaseNode() = default;

    // A method to add a child node, useful for building the tree
    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

private:
    std::vector<std::unique_ptr<BaseNode>> children;
};