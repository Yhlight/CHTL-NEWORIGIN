#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    std::string content;

    ScriptNode(const std::string& content = "") : content(content) {
        type = NodeType::NODE_SCRIPT;
    }

    const std::string& getContent() const { return content; }
};

}