#include "CHTLNode.h"
#include <iostream>
#include <sstream>

CHTLNode::CHTLNode(NodeType type, const std::string& value) 
    : type_(type), value_(value), line_(0), column_(0) {
}

void CHTLNode::addChild(std::unique_ptr<CHTLNode> child) {
    children_.push_back(std::move(child));
}

void CHTLNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

const std::string& CHTLNode::getAttribute(const std::string& name) const {
    static const std::string empty;
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : empty;
}

bool CHTLNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void CHTLNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string CHTLNode::toString() const {
    std::stringstream ss;
    ss << "CHTLNode(" << static_cast<int>(type_) << ", \"" << value_ << "\")";
    return ss.str();
}

std::string CHTLNode::toDebugString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 2, ' ');
    
    ss << indentStr << "CHTLNode {\n";
    ss << indentStr << "  type: " << static_cast<int>(type_) << "\n";
    ss << indentStr << "  value: \"" << value_ << "\"\n";
    ss << indentStr << "  line: " << line_ << ", column: " << column_ << "\n";
    
    if (!attributes_.empty()) {
        ss << indentStr << "  attributes: {\n";
        for (const auto& attr : attributes_) {
            ss << indentStr << "    " << attr.first << ": \"" << attr.second << "\"\n";
        }
        ss << indentStr << "  }\n";
    }
    
    if (!children_.empty()) {
        ss << indentStr << "  children: [\n";
        for (const auto& child : children_) {
            ss << child->toDebugString(indent + 1) << "\n";
        }
        ss << indentStr << "  ]\n";
    }
    
    ss << indentStr << "}";
    return ss.str();
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tagName) 
    : CHTLNode(NodeType::ELEMENT), tagName_(tagName) {
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode(\"" << tagName_ << "\")";
    return ss.str();
}

// TextNode 实现
TextNode::TextNode(const std::string& text) 
    : CHTLNode(NodeType::TEXT, text) {
}

std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "TextNode(\"" << value_ << "\")";
    return ss.str();
}

// TemplateNode 实现
TemplateNode::TemplateNode(const std::string& templateType, const std::string& name) 
    : CHTLNode(NodeType::TEMPLATE), templateType_(templateType), templateName_(name) {
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode(\"" << templateType_ << "\", \"" << templateName_ << "\")";
    return ss.str();
}

// CustomNode 实现
CustomNode::CustomNode(const std::string& customType, const std::string& name) 
    : CHTLNode(NodeType::CUSTOM), customType_(customType), customName_(name) {
}

std::string CustomNode::toString() const {
    std::stringstream ss;
    ss << "CustomNode(\"" << customType_ << "\", \"" << customName_ << "\")";
    return ss.str();
}

// StyleNode 实现
StyleNode::StyleNode() 
    : CHTLNode(NodeType::STYLE) {
}

std::string StyleNode::toString() const {
    return "StyleNode()";
}

// ScriptNode 实现
ScriptNode::ScriptNode() 
    : CHTLNode(NodeType::SCRIPT) {
}

std::string ScriptNode::toString() const {
    return "ScriptNode()";
}

// ExpressionNode 实现
ExpressionNode::ExpressionNode(const std::string& expression) 
    : CHTLNode(NodeType::EXPRESSION, expression) {
}

std::string ExpressionNode::toString() const {
    std::stringstream ss;
    ss << "ExpressionNode(\"" << value_ << "\")";
    return ss.str();
}

// ConditionalNode 实现
ConditionalNode::ConditionalNode(std::unique_ptr<CHTLNode> condition, 
                                std::unique_ptr<CHTLNode> trueExpr, 
                                std::unique_ptr<CHTLNode> falseExpr) 
    : CHTLNode(NodeType::CONDITIONAL) {
    addChild(std::move(condition));
    addChild(std::move(trueExpr));
    addChild(std::move(falseExpr));
}

std::string ConditionalNode::toString() const {
    return "ConditionalNode()";
}

// OperationNode 实现
OperationNode::OperationNode(const std::string& operator_, 
                            std::unique_ptr<CHTLNode> left, 
                            std::unique_ptr<CHTLNode> right) 
    : CHTLNode(NodeType::OPERATION), operator_(operator_) {
    addChild(std::move(left));
    addChild(std::move(right));
}

std::string OperationNode::toString() const {
    std::stringstream ss;
    ss << "OperationNode(\"" << operator_ << "\")";
    return ss.str();
}