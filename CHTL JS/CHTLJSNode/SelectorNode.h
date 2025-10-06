#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTLJS {

class SelectorNode : public BaseNode {
public:
    std::string selector;

    SelectorNode(const std::string& selector) : selector(selector) {
        type = NodeType::NODE_SELECTOR;
    }

    const std::string& getSelector() const { return selector; }
};

}