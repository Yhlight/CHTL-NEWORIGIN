#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    const std::string& getTagName() const { return tagName; }

    std::string getNodeType() const override { return "@Html"; }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    const std::map<std::string, std::string>& getAttributes() const { return attributes; }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "<" << tagName;
        for (const auto& attr : attributes) {
            ss << " " << attr.first << "=\"" << attr.second << "\"";
        }
        ss << ">" << std::endl;

        for (const auto& child : children) {
            ss << child->toString(depth + 1);
        }

        ss << std::string(depth * 2, ' ') << "</" << tagName << ">" << std::endl;
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

public:
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

    void addConstraint(const std::string& constraint) {
        constraints.push_back(constraint);
    }

    const std::vector<std::string>& getConstraints() const {
        return constraints;
    }

    void addTypeConstraint(const std::string& typeConstraint) {
        type_constraints.push_back(typeConstraint);
    }

    const std::vector<std::string>& getTypeConstraints() const {
        return type_constraints;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::vector<std::string> constraints;
    std::vector<std::string> type_constraints;
};

#endif //CHTL_ELEMENT_NODE_H