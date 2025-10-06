#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTLJS {

class StringLiteralNode : public BaseNode {
public:
    std::string value;

    StringLiteralNode(const std::string& value) : value(value) {
        type = NodeType::NODE_STRING_LITERAL;
    }

    const std::string& getValue() const { return value; }
};

}