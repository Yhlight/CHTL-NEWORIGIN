#pragma once

#include "Node.h"

#include "../CHTLGenerator/Visitor.h"

class RootNode : public Node {
public:
    RootNode(NodeList children) : children(std::move(children)) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    const NodeList& getChildren() const { return children; }

private:
    NodeList children;
};
