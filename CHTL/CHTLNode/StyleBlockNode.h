#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

class StyleBlockNode : public BaseNode {
public:
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
    // The `children` vector inherited from BaseNode will be used
    // to store std::shared_ptr<StylePropertyNode>.
};
