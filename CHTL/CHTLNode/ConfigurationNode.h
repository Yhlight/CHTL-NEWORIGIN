#pragma once

#include "BaseNode.h"
#include "ConfigPropertyNode.h"
#include <vector>
#include <memory>

class ConfigurationNode : public BaseNode {
public:
    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
    // The `children` vector from BaseNode will be used
    // to store std::shared_ptr<ConfigPropertyNode>.
};
