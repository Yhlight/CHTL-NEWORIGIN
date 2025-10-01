#ifndef CHTL_VALUE_LIST_NODE_H
#define CHTL_VALUE_LIST_NODE_H

#include <vector>
#include <memory>
#include "ExpressionNode.h"

namespace CHTL {

class CHTLGenerator;

class ValueListNode : public ExpressionNode {
public:
    std::vector<std::unique_ptr<ExpressionNode>> values;

    explicit ValueListNode(std::vector<std::unique_ptr<ExpressionNode>> values)
        : values(std::move(values)) {}

    ExpressionType getType() const override { return ExpressionType::VALUE_LIST; }
    void print(int indent = 0) const override;
};

}

#endif // CHTL_VALUE_LIST_NODE_H