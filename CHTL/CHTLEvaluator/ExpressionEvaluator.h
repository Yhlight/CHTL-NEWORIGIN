#pragma once

#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/LiteralExpressionNode.h"
#include "../CHTLNode/UnaryExpressionNode.h"
#include "../CHTLNode/BinaryExpressionNode.h"
#include "../CHTLNode/GroupingExpressionNode.h"
#include "../CHTLNode/VariableExpressionNode.h"
#include "../CHTLNode/ElementNode.h"
#include <memory>
#include <string>
#include <any>

namespace CHTL {

    class ExpressionEvaluator {
    public:
        ExpressionEvaluator(const std::shared_ptr<ElementNode>& context_element);
        std::any evaluate(const std::shared_ptr<ExpressionNode>& expr);

    private:
        const std::shared_ptr<ElementNode> context_element;

        std::any visit(const std::shared_ptr<ExpressionNode>& expr);
        std::any visitLiteral(const std::shared_ptr<LiteralExpressionNode>& expr);
        std::any visitUnary(const std::shared_ptr<UnaryExpressionNode>& expr);
        std::any visitBinary(const std::shared_ptr<BinaryExpressionNode>& expr);
        std::any visitGrouping(const std::shared_ptr<GroupingExpressionNode>& expr);
        std::any visitVariable(const std::shared_ptr<VariableExpressionNode>& expr);

        bool isTruthy(const std::any& value);
        double toDouble(const std::any& value);
    };

}