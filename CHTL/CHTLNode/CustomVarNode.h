#ifndef CHTL_CUSTOM_VAR_NODE_H
#define CHTL_CUSTOM_VAR_NODE_H

#include "TemplateVarNode.h"

class CustomVarNode : public TemplateVarNode {
public:
    explicit CustomVarNode(const std::string& name)
        : TemplateVarNode(name) {}
};

#endif // CHTL_CUSTOM_VAR_NODE_H