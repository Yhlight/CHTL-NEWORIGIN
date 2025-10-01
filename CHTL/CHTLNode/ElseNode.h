#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ElseNode : public BaseNode {
public:
    ElseNode(std::vector<std::unique_ptr<StylePropertyNode>> body);
    void print(int indent = 0) const override;

    const std::vector<std::unique_ptr<StylePropertyNode>>& getBody() const { return body; }

private:
    std::vector<std::unique_ptr<StylePropertyNode>> body;
};

} // namespace CHTL