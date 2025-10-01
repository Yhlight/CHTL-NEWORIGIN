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

    void print(int indent = 0) const override;
    ExpressionType getType() const override { return ExpressionType::VALUE_LIST; }
};

}

#endif // CHTL_VALUE_LIST_NODE_H