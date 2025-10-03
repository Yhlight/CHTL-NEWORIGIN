#ifndef CHTL_DOCUMENT_NODE_H
#define CHTL_DOCUMENT_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <vector>
#include <memory>
#include <sstream>

class DocumentNode : public BaseNode {
public:
    DocumentNode() : hasHtml5Doctype(false) {}

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

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

    void setHasHtml5Doctype(bool value) {
        hasHtml5Doctype = value;
    }

    bool getHasHtml5Doctype() const {
        return hasHtml5Doctype;
    }

private:
    std::vector<std::unique_ptr<BaseNode>> children;
    bool hasHtml5Doctype;
};

#endif //CHTL_DOCUMENT_NODE_H