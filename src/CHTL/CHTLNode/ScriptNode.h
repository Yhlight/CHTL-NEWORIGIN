#pragma once

#include "BaseNode.h"
#include <string>

class ScriptNode : public BaseNode {
public:
    ScriptNode(const std::string& script) : script(script) {}

    NodeType getType() const override { return NodeType::Script; }

    const std::string& getScript() const {
        return script;
    }

private:
    std::string script;
};