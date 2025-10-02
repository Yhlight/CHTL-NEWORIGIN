#ifndef CHTL_USE_NODE_H
#define CHTL_USE_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>

class UseNode : public BaseNode {
public:
    explicit UseNode(std::string directive) : directive(std::move(directive)) {}

    std::string toString(int depth = 0) const override {
        return std::string(depth * 2, ' ') + "UseNode: " + directive + "\n";
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    const std::string& getDirective() const {
        return directive;
    }

private:
    std::string directive;
};

#endif //CHTL_USE_NODE_H