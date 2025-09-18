#include "ExpressionEvaluator.h"
#include "../CHTLNode/Expression/LiteralExprNode.h"
#include "../CHTLNode/Expression/BinaryOpExprNode.h"
#include "../CHTLNode/Expression/PropertyAccessExprNode.h"
#include "../CHTLNode/Expression/GroupExprNode.h"
#include "../CHTLNode/ElementNode.h" // Needed for findNode
#include "../CHTLNode/StyleNode.h"   // Needed for dynamic_cast and member access
#include <stdexcept>
#include <charconv> // For std::from_chars

namespace CHTL
{
    // Helper to parse a literal string like "100px" into a value and a unit.
    EvaluatedValue parseLiteral(const std::string& literal) {
        EvaluatedValue val;
        double num;
        auto [ptr, ec] = std::from_chars(literal.data(), literal.data() + literal.size(), num);

        if (ec == std::errc()) {
            val.isNumeric = true;
            val.numericValue = num;
            val.unit = std::string(ptr, literal.data() + literal.size() - ptr);
        } else {
            val.isNumeric = false;
            val.stringValue = literal;
        }
        return val;
    }

    EvaluatedValue ExpressionEvaluator::evaluate(ExprNode &node, const std::vector<std::unique_ptr<BaseNode>>* context, int depth)
    {
        if (depth > 50) { // Max recursion depth to prevent stack overflow
            throw std::runtime_error("Maximum evaluation depth exceeded, circular reference suspected.");
        }
        this->astContext = context;
        this->currentDepth = depth;
        node.accept(*this);
        return lastValue;
    }

    void ExpressionEvaluator::visit(LiteralExprNode &node)
    {
        lastValue = parseLiteral(node.literal.lexeme);
    }

    void ExpressionEvaluator::visit(BinaryOpExprNode &node)
    {
        EvaluatedValue left = evaluate(*node.left, this->astContext, this->currentDepth + 1);
        EvaluatedValue right = evaluate(*node.right, this->astContext, this->currentDepth + 1);

        if (!left.isNumeric || !right.isNumeric) {
            throw std::runtime_error("Cannot perform arithmetic on non-numeric values.");
        }

        // Basic unit check: for now, either units must match or one can be empty.
        if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
            throw std::runtime_error("Incompatible units for arithmetic: '" + left.unit + "' and '" + right.unit + "'.");
        }

        EvaluatedValue result;
        result.isNumeric = true;
        result.unit = !left.unit.empty() ? left.unit : right.unit;

        switch(node.op.type) {
            case TokenType::TOKEN_PLUS:
                result.numericValue = left.numericValue + right.numericValue;
                break;
            case TokenType::TOKEN_MINUS:
                result.numericValue = left.numericValue - right.numericValue;
                break;
            case TokenType::TOKEN_STAR:
                result.numericValue = left.numericValue * right.numericValue;
                break;
            case TokenType::TOKEN_SLASH:
                if (right.numericValue == 0) throw std::runtime_error("Division by zero.");
                result.numericValue = left.numericValue / right.numericValue;
                break;
            default:
                throw std::runtime_error("Unsupported binary operator.");
        }

        lastValue = result;
    }

    void ExpressionEvaluator::visit(GroupExprNode &node)
    {
        lastValue = evaluate(*node.expression, this->astContext, this->currentDepth + 1);
    }

    void ExpressionEvaluator::visit(PropertyAccessExprNode &node)
    {
        // 1. Find the selector string (e.g., "box")
        auto objectLiteral = dynamic_cast<LiteralExprNode*>(node.object.get());
        if (!objectLiteral) {
            throw std::runtime_error("Property access on a non-literal object is not supported.");
        }
        std::string selector = objectLiteral->literal.lexeme;

        // 2. Find the target node in the AST
        if (!this->astContext) {
            throw std::runtime_error("AST context not provided for property access evaluation.");
        }
        ElementNode* targetNode = findNode(selector, *this->astContext);
        if (!targetNode) {
            throw std::runtime_error("Could not find element with selector: " + selector);
        }

        // 3. Find the StyleNode child of the target node
        StyleNode* targetStyleNode = nullptr;
        for (const auto& child : targetNode->children) {
            if (StyleNode* sn = dynamic_cast<StyleNode*>(child.get())) {
                targetStyleNode = sn;
                break;
            }
        }
        if (!targetStyleNode) {
            throw std::runtime_error("Referenced element '" + selector + "' has no style block.");
        }

        // 4. Find the property expression in the StyleNode
        auto it = targetStyleNode->properties.find(node.property.lexeme);
        if (it == targetStyleNode->properties.end()) {
            throw std::runtime_error("Property '" + node.property.lexeme + "' not found on element '" + selector + "'.");
        }

        // 5. Recursively evaluate the found property's expression
        lastValue = evaluate(*it->second, this->astContext, this->currentDepth + 1);
    }

    ElementNode* ExpressionEvaluator::findNode(const std::string& selector, const std::vector<std::unique_ptr<BaseNode>>& nodes) {
        for(const auto& node : nodes) {
            if (ElementNode* element = dynamic_cast<ElementNode*>(node.get())) {
                // For now, only check for ID.
                auto it = element->attributes.find("id");
                if (it != element->attributes.end() && it->second == selector) {
                    return element;
                }

                // Recurse into children
                if (ElementNode* found = findNode(selector, element->children)) {
                    return found;
                }
            }
        }
        return nullptr;
    }
}
