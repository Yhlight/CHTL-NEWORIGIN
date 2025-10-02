#ifndef CHTL_DECLARATION_NODE_H
#define CHTL_DECLARATION_NODE_H

#include "BaseNode.h"
#include <string>

class DeclarationNode : public BaseNode {
public:
    explicit DeclarationNode(std::string type) : declarationType(std::move(type)) {}

    std::string toString(int depth = 0) const override {
        // Declarations do not render to the final output.
        return "";
    }

    std::string getNodeType() const override {
        return declarationType;
    }

private:
    std::string declarationType;
};

#endif //CHTL_DECLARATION_NODE_H