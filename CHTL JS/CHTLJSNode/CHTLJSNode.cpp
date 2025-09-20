#include "CHTLJSNode.h"
#include <iostream>
#include <sstream>

CHTLJSNode::CHTLJSNode(CHTLJSNodeType type, const std::string& value) 
    : type_(type), value_(value), line_(0), column_(0) {
}

void CHTLJSNode::addChild(std::unique_ptr<CHTLJSNode> child) {
    children_.push_back(std::move(child));
}

void CHTLJSNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

const std::string& CHTLJSNode::getAttribute(const std::string& name) const {
    static const std::string empty;
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : empty;
}

bool CHTLJSNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void CHTLJSNode::accept(CHTLJSNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string CHTLJSNode::toString() const {
    std::stringstream ss;
    ss << "CHTLJSNode(" << static_cast<int>(type_) << ", \"" << value_ << "\")";
    return ss.str();
}

std::string CHTLJSNode::toDebugString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 2, ' ');
    
    ss << indentStr << "CHTLJSNode {\n";
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

// ListenNode 实现
ListenNode::ListenNode() 
    : CHTLJSNode(CHTLJSNodeType::LISTEN) {
}

std::string ListenNode::toString() const {
    return "ListenNode()";
}

// AnimateNode 实现
AnimateNode::AnimateNode() 
    : CHTLJSNode(CHTLJSNodeType::ANIMATE) {
}

std::string AnimateNode::toString() const {
    return "AnimateNode()";
}

// RouterNode 实现
RouterNode::RouterNode() 
    : CHTLJSNode(CHTLJSNodeType::ROUTER) {
}

std::string RouterNode::toString() const {
    return "RouterNode()";
}

// SelectorNode 实现
SelectorNode::SelectorNode(const std::string& selector) 
    : CHTLJSNode(CHTLJSNodeType::SELECTOR), selector_(selector) {
}

std::string SelectorNode::toString() const {
    std::stringstream ss;
    ss << "SelectorNode(\"" << selector_ << "\")";
    return ss.str();
}

// EventBindingNode 实现
EventBindingNode::EventBindingNode(const std::string& eventType, const std::string& handler) 
    : CHTLJSNode(CHTLJSNodeType::EVENT_BINDING), eventType_(eventType), handler_(handler) {
}

std::string EventBindingNode::toString() const {
    std::stringstream ss;
    ss << "EventBindingNode(\"" << eventType_ << "\", \"" << handler_ << "\")";
    return ss.str();
}

// AnimationNode 实现
AnimationNode::AnimationNode() 
    : CHTLJSNode(CHTLJSNodeType::ANIMATION) {
}

std::string AnimationNode::toString() const {
    return "AnimationNode()";
}

// RouteNode 实现
RouteNode::RouteNode(const std::string& url, const std::string& page) 
    : CHTLJSNode(CHTLJSNodeType::ROUTE), url_(url), page_(page) {
}

std::string RouteNode::toString() const {
    std::stringstream ss;
    ss << "RouteNode(\"" << url_ << "\", \"" << page_ << "\")";
    return ss.str();
}

// VirtualObjectNode 实现
VirtualObjectNode::VirtualObjectNode(const std::string& name) 
    : CHTLJSNode(CHTLJSNodeType::VIRTUAL_OBJECT), name_(name) {
}

std::string VirtualObjectNode::toString() const {
    std::stringstream ss;
    ss << "VirtualObjectNode(\"" << name_ << "\")";
    return ss.str();
}