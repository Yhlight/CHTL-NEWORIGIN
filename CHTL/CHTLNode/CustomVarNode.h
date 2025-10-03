#ifndef CHTL_CUSTOM_VAR_NODE_H
#define CHTL_CUSTOM_VAR_NODE_H

#include "TemplateVarNode.h"
#include "NodeVisitor.h"

class CustomVarNode : public TemplateVarNode {
public:
    explicit CustomVarNode(const std::string& name)
        : TemplateVarNode(name) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

#endif // CHTL_CUSTOM_VAR_NODE_H