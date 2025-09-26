#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <vector>

class ScriptLoaderNode : public CHTLJSNode {
public:
    ScriptLoaderNode() = default;

    CHTLJSNodeType getType() const override {
        return CHTLJSNodeType::ScriptLoader;
    }

    std::vector<std::string> paths;
};