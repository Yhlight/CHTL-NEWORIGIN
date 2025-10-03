#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include <iostream>

class ExpressionNode;

class ASTPrinter {
public:
    void print(BaseNode* root);

private:
    void printNode(BaseNode* node, int indent);
    void printExpression(const ExpressionNode* node, int indent);
};