#ifndef CHTL_TEMPLATE_STYLE_NODE_H
#define CHTL_TEMPLATE_STYLE_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <utility>
#include <sstream>

class TemplateStyleNode : public BaseNode {
public:
    TemplateStyleNode(const std::string& name) : templateName(name) {}

    void addProperty(const std::string& key, const std::string& value) {
        properties.push_back({key, value});
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "[Template] @Style " << templateName << " {" << std::endl;
        for (const auto& prop : properties) {
            ss << std::string((depth + 1) * 2, ' ') << prop.first << ": " << prop.second << ";" << std::endl;
        }
        ss << std::string(depth * 2, ' ') << "}" << std::endl;
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<TemplateStyleNode>(templateName);
        for (const auto& prop : properties) {
            clonedNode->addProperty(prop.first, prop.second);
        }
        return clonedNode;
    }

    const std::string& getName() const { return templateName; }
    const std::vector<std::pair<std::string, std::string>>& getProperties() const { return properties; }

    std::string getNodeType() const override { return "[Template] @Style"; }

private:
    std::string templateName;
    std::vector<std::pair<std::string, std::string>> properties;
};

#endif //CHTL_TEMPLATE_STYLE_NODE_H