#ifndef CHTL_TEMPLATE_ELEMENT_NODE_H
#define CHTL_TEMPLATE_ELEMENT_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class TemplateElementNode : public BaseNode {
public:
    TemplateElementNode(const std::string& name) : templateName(name) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "[Template] @Element " << templateName << " {" << std::endl;
        for (const auto& child : children) {
            ss << child->toString(depth + 1);
        }
        ss << std::string(depth * 2, ' ') << "}" << std::endl;
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    const std::string& getName() const { return templateName; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }

    std::string getNodeType() const override { return "[Template] @Element"; }

private:
    std::string templateName;
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif //CHTL_TEMPLATE_ELEMENT_NODE_H