#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

class ConditionalBlockNode : public BaseNode {
public:
    ConditionalBlockNode(const std::string& type, const std::string& condition = "");

    virtual NodeType getType() const override;
    std::string getConditionalType() const;
    std::string getCondition() const;
    void setCondition(const std::string& newCondition);

    virtual std::unique_ptr<BaseNode> clone() const override;

private:
    std::string conditionalType; // "if", "else if", "else"
    std::string condition;
};