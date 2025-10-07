/**
 * @file ConditionalNode.cpp
 * @brief Implementation of ConditionalNode
 */

#include "ConditionalNode.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <regex>

namespace CHTL {

// ============================================================================
// ConditionalNode Implementation
// ============================================================================

ConditionalNode::ConditionalNode() 
    : BaseNode(NodeType::Conditional)
    , condition_("")
    , styles_()
    , elseIfBlocks_()
    , elseBlock_(nullptr)
    , isDynamic_(false)
    , blockType_(BlockType::If)
    , parentSelector_("") {
}

void ConditionalNode::accept(NodeVisitor& /* visitor */) {
    // TODO: Implement visitor pattern for ConditionalNode
}

SharedPtr<BaseNode> ConditionalNode::clone() const {
    auto node = std::make_shared<ConditionalNode>();
    node->condition_ = condition_;
    node->styles_ = styles_;
    node->isDynamic_ = isDynamic_;
    node->blockType_ = blockType_;
    node->parentSelector_ = parentSelector_;
    
    // Clone else-if blocks
    for (const auto& elseIf : elseIfBlocks_) {
        node->elseIfBlocks_.push_back(std::static_pointer_cast<ConditionalNode>(elseIf->clone()));
    }
    
    // Clone else block
    if (elseBlock_) {
        node->elseBlock_ = std::static_pointer_cast<ConditionalNode>(elseBlock_->clone());
    }
    
    return node;
}

void ConditionalNode::addStyle(const String& property, const String& value) {
    styles_.push_back({property, value});
}

void ConditionalNode::addElseIfBlock(SharedPtr<ConditionalNode> elseIfNode) {
    if (elseIfNode) {
        elseIfNode->setBlockType(BlockType::ElseIf);
        elseIfBlocks_.push_back(elseIfNode);
    }
}

void ConditionalNode::setElseBlock(SharedPtr<ConditionalNode> elseNode) {
    if (elseNode) {
        elseNode->setBlockType(BlockType::Else);
        elseBlock_ = elseNode;
    }
}

Vector<String> ConditionalNode::extractEnhancedSelectors() const {
    return ConditionalUtils::extractEnhancedSelectors(condition_);
}

bool ConditionalNode::hasArithmeticInCondition() const {
    // Check for arithmetic operators: +, -, *, /, %, **
    return condition_.find('+') != String::npos ||
           condition_.find('-') != String::npos ||
           condition_.find('*') != String::npos ||
           condition_.find('/') != String::npos ||
           condition_.find('%') != String::npos;
}

bool ConditionalNode::hasLogicalOperators() const {
    // Check for logical operators: &&, ||
    return condition_.find("&&") != String::npos ||
           condition_.find("||") != String::npos;
}

// ============================================================================
// ConditionalUtils Implementation
// ============================================================================

bool ConditionalUtils::containsEnhancedSelector(const String& condition) {
    return condition.find("{{") != String::npos && 
           condition.find("}}") != String::npos;
}

Vector<String> ConditionalUtils::extractEnhancedSelectors(const String& condition) {
    Vector<String> selectors;
    
    size_t pos = 0;
    while ((pos = condition.find("{{", pos)) != String::npos) {
        size_t end = condition.find("}}", pos);
        if (end != String::npos) {
            // Extract content between {{ and }}
            String selector = condition.substr(pos + 2, end - pos - 2);
            
            // Remove any whitespace
            selector = Util::StringUtil::trim(selector);
            
            selectors.push_back(selector);
            pos = end + 2;
        } else {
            break;
        }
    }
    
    return selectors;
}

bool ConditionalUtils::isValidCondition(const String& condition) {
    if (condition.empty()) {
        return false;
    }
    
    // Check for basic structure: operand operator operand
    // Valid operators: >, <, >=, <=, ==, !=, &&, ||
    static const Vector<String> operators = {
        ">=", "<=", "==", "!=", ">", "<", "&&", "||"
    };
    
    for (const auto& op : operators) {
        if (condition.find(op) != String::npos) {
            return true;
        }
    }
    
    return false;
}

Tuple<String, String, String> ConditionalUtils::parseCondition(const String& condition) {
    // Parse condition into (left operand, operator, right operand)
    
    // List of operators in order of precedence (longest first to match correctly)
    static const Vector<String> operators = {
        ">=", "<=", "==", "!=", ">", "<"
    };
    
    for (const auto& op : operators) {
        size_t pos = condition.find(op);
        if (pos != String::npos) {
            String left = Util::StringUtil::trim(condition.substr(0, pos));
            String right = Util::StringUtil::trim(condition.substr(pos + op.length()));
            return std::make_tuple(left, op, right);
        }
    }
    
    // If no operator found, return empty tuple
    return std::make_tuple("", "", "");
}

} // namespace CHTL
