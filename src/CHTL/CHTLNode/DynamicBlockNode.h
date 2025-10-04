#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

class DynamicBlockNode : public BaseNode {
public:
    DynamicBlockNode(const std::string& type, const std::string& condition);

    virtual NodeType getType() const override;
    std::string getConditionalType() const;
    std::string getCondition() const;

    virtual std::unique_ptr<BaseNode> clone() const override;

private:
    std::string conditionalType; // "if", "else if", "else"
    std::string condition;
};