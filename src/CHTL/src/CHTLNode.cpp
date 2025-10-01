#include "CHTLNode.h"
#include "CHTLContext.h"

namespace CHTL {

// Base CHTLNode implementation
CHTLNode::CHTLNode(Type type, const std::string& name)
    : type_(type), name_(name), line_(-1), column_(-1) {
}

void CHTLNode::addChild(std::unique_ptr<CHTLNode> child) {
    children_.push_back(std::move(child));
}

CHTLNode* CHTLNode::getChild(size_t index) const {
    return (index < children_.size()) ? children_[index].get() : nullptr;
}

size_t CHTLNode::getChildCount() const {
    return children_.size();
}

std::vector<CHTLNode*> CHTLNode::getChildren() const {
    std::vector<CHTLNode*> result;
    for (const auto& child : children_) {
        result.push_back(child.get());
    }
    return result;
}

void CHTLNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string CHTLNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : "";
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

const std::unordered_map<std::string, std::string>& CHTLNode::getAttributes() const {
    return attributes_;
}

void CHTLNode::setPosition(int line, int column) {
    line_ = line;
    column_ = column;
}

bool CHTLNode::validate(CHTLContext& context) const {
    // Base validation - can be overridden by derived classes
    return true;
}

void CHTLNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// ElementNode implementation
ElementNode::ElementNode(const std::string& tagName)
    : CHTLNode(Type::ELEMENT, tagName) {
}

bool ElementNode::validate(CHTLContext& context) const {
    // Basic element validation
    if (getName().empty()) {
        context.addError("Element tag name cannot be empty", getLine(), getColumn());
        return false;
    }
    return CHTLNode::validate(context);
}

void ElementNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// TextNode implementation
TextNode::TextNode(const std::string& content)
    : CHTLNode(Type::TEXT, content) {
}

bool TextNode::validate(CHTLContext& context) const {
    // Basic text validation
    return CHTLNode::validate(context);
}

void TextNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// TemplateNode implementation
TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : CHTLNode(Type::TEMPLATE, name), templateType_(type) {
}

bool TemplateNode::validate(CHTLContext& context) const {
    if (getName().empty()) {
        context.addError("Template name cannot be empty", getLine(), getColumn());
        return false;
    }
    return CHTLNode::validate(context);
}

void TemplateNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// CustomNode implementation
CustomNode::CustomNode(CustomType type, const std::string& name)
    : CHTLNode(Type::CUSTOM, name), customType_(type) {
}

bool CustomNode::validate(CHTLContext& context) const {
    if (getName().empty()) {
        context.addError("Custom name cannot be empty", getLine(), getColumn());
        return false;
    }
    return CHTLNode::validate(context);
}

void CustomNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// StyleNode implementation
StyleNode::StyleNode()
    : CHTLNode(Type::STYLE, "style") {
}

bool StyleNode::validate(CHTLContext& context) const {
    return CHTLNode::validate(context);
}

void StyleNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// ScriptNode implementation
ScriptNode::ScriptNode()
    : CHTLNode(Type::SCRIPT, "script") {
}

bool ScriptNode::validate(CHTLContext& context) const {
    return CHTLNode::validate(context);
}

void ScriptNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

// ImportNode implementation
ImportNode::ImportNode(ImportType type, const std::string& path)
    : CHTLNode(Type::IMPORT, path), importType_(type) {
}

bool ImportNode::validate(CHTLContext& context) const {
    if (getName().empty()) {
        context.addError("Import path cannot be empty", getLine(), getColumn());
        return false;
    }
    return CHTLNode::validate(context);
}

void ImportNode::accept(CHTLNodeVisitor& visitor) const {
    visitor.visit(*this);
}

} // namespace CHTL