#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& originType, const std::string& name, const std::string& content)
        : originType(originType), name(name), content(content) {
        type = NodeType::NODE_ORIGIN;
    }

    const std::string& getOriginType() const { return originType; }
    const std::string& getName() const { return name; }
    const std::string& getContent() const { return content; }

private:
    std::string originType;
    std::string name;
    std::string content;
};

}