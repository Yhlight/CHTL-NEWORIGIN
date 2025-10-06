#include "BaseNode.h"
#include <sstream>

namespace CHTL {

// BaseNode实现
BaseNode::BaseNode(NodeType type, const SourceRange& range)
    : type_(type), range_(range) {}

void BaseNode::addChild(SharedPtr<BaseNode> child) {
    if (child) {
        child->setParent(this);
        children_.push_back(child);
    }
}

String BaseNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "BaseNode(" << static_cast<int>(type_) << ")";
    return ss.str();
}

String BaseNode::indentString(int indent) const {
    return String(indent * 2, ' ');
}

// ProgramNode实现
ProgramNode::ProgramNode() : BaseNode(NodeType::Program) {}

void ProgramNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ProgramNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Program\n";
    for (const auto& child : children_) {
        ss << child->toString(indent + 1) << "\n";
    }
    return ss.str();
}

SharedPtr<BaseNode> ProgramNode::clone() const {
    auto node = std::make_shared<ProgramNode>();
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

// ElementNode实现
ElementNode::ElementNode(const String& tagName, const SourceRange& range)
    : BaseNode(NodeType::Element, range), tagName_(tagName) {}

void ElementNode::setAttribute(const String& name, const String& value) {
    attributes_[name] = value;
}

Optional<String> ElementNode::getAttribute(const String& name) const {
    auto it = attributes_.find(name);
    if (it != attributes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool ElementNode::hasAttribute(const String& name) const {
    return attributes_.find(name) != attributes_.end();
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ElementNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Element(" << tagName_ << ")";
    
    if (!attributes_.empty()) {
        ss << " [";
        bool first = true;
        for (const auto& [name, value] : attributes_) {
            if (!first) ss << ", ";
            ss << name << "=\"" << value << "\"";
            first = false;
        }
        ss << "]";
    }
    
    if (!children_.empty()) {
        ss << "\n";
        for (const auto& child : children_) {
            ss << child->toString(indent + 1) << "\n";
        }
    }
    
    return ss.str();
}

SharedPtr<BaseNode> ElementNode::clone() const {
    auto node = std::make_shared<ElementNode>(tagName_, range_);
    node->attributes_ = attributes_;
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

// TextNode实现
TextNode::TextNode(const String& content, const SourceRange& range)
    : BaseNode(NodeType::Text, range), content_(content) {}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String TextNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Text(\"" << content_ << "\")";
    return ss.str();
}

SharedPtr<BaseNode> TextNode::clone() const {
    return std::make_shared<TextNode>(content_, range_);
}

// CommentNode实现
CommentNode::CommentNode(const String& content, CommentType commentType, 
                         const SourceRange& range)
    : BaseNode(NodeType::Comment, range)
    , content_(content)
    , commentType_(commentType) {}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String CommentNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Comment(";
    switch (commentType_) {
        case CommentType::Line: ss << "Line"; break;
        case CommentType::Block: ss << "Block"; break;
        case CommentType::Generator: ss << "Generator"; break;
    }
    ss << ", \"" << content_ << "\")";
    return ss.str();
}

SharedPtr<BaseNode> CommentNode::clone() const {
    return std::make_shared<CommentNode>(content_, commentType_, range_);
}

// AttributeNode实现
AttributeNode::AttributeNode(const String& name, const String& value,
                             const SourceRange& range)
    : BaseNode(NodeType::Attribute, range), name_(name), value_(value) {}

void AttributeNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String AttributeNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Attribute(" << name_ << "=\"" << value_ << "\")";
    return ss.str();
}

SharedPtr<BaseNode> AttributeNode::clone() const {
    return std::make_shared<AttributeNode>(name_, value_, range_);
}

// StyleNode实现
StyleNode::StyleNode(const SourceRange& range)
    : BaseNode(NodeType::Style, range) {}

void StyleNode::addInlineStyle(const String& property, const String& value) {
    inlineStyles_[property] = value;
}

void StyleNode::addCssRule(const String& selector, const String& rules) {
    cssRules_[selector] = rules;
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String StyleNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Style";
    
    if (!inlineStyles_.empty()) {
        ss << " Inline[";
        bool first = true;
        for (const auto& [prop, val] : inlineStyles_) {
            if (!first) ss << ", ";
            ss << prop << ": " << val;
            first = false;
        }
        ss << "]";
    }
    
    if (!cssRules_.empty()) {
        ss << " CSS[";
        bool first = true;
        for (const auto& [sel, rules] : cssRules_) {
            if (!first) ss << ", ";
            ss << sel << " {...}";
            first = false;
        }
        ss << "]";
    }
    
    return ss.str();
}

SharedPtr<BaseNode> StyleNode::clone() const {
    auto node = std::make_shared<StyleNode>(range_);
    node->inlineStyles_ = inlineStyles_;
    node->cssRules_ = cssRules_;
    return node;
}

// ScriptNode实现
ScriptNode::ScriptNode(const String& content, const SourceRange& range)
    : BaseNode(NodeType::Script, range), content_(content) {}

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ScriptNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Script";
    if (isLocal_) ss << "(local)";
    ss << " {" << content_.substr(0, 50);
    if (content_.length() > 50) ss << "...";
    ss << "}";
    return ss.str();
}

SharedPtr<BaseNode> ScriptNode::clone() const {
    auto node = std::make_shared<ScriptNode>(content_, range_);
    node->isLocal_ = isLocal_;
    return node;
}

// TemplateNode实现
TemplateNode::TemplateNode(TemplateType templateType, const String& name,
                           const SourceRange& range)
    : BaseNode(NodeType::Template, range)
    , templateType_(templateType)
    , name_(name) {}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String TemplateNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Template(";
    switch (templateType_) {
        case TemplateType::Style: ss << "@Style"; break;
        case TemplateType::Element: ss << "@Element"; break;
        case TemplateType::Var: ss << "@Var"; break;
    }
    ss << ", " << name_ << ")";
    return ss.str();
}

SharedPtr<BaseNode> TemplateNode::clone() const {
    auto node = std::make_shared<TemplateNode>(templateType_, name_, range_);
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

// CustomNode实现
CustomNode::CustomNode(CustomType customType, const String& name,
                       const SourceRange& range)
    : BaseNode(NodeType::Custom, range)
    , customType_(customType)
    , name_(name) {}

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String CustomNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Custom(";
    switch (customType_) {
        case CustomType::Style: ss << "@Style"; break;
        case CustomType::Element: ss << "@Element"; break;
        case CustomType::Var: ss << "@Var"; break;
    }
    ss << ", " << name_ << ")";
    return ss.str();
}

SharedPtr<BaseNode> CustomNode::clone() const {
    auto node = std::make_shared<CustomNode>(customType_, name_, range_);
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

// OriginNode实现
OriginNode::OriginNode(OriginType originType, const String& content,
                       const SourceRange& range)
    : BaseNode(NodeType::Origin, range)
    , originType_(originType)
    , content_(content) {}

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String OriginNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Origin(";
    switch (originType_) {
        case OriginType::Html: ss << "@Html"; break;
        case OriginType::Style: ss << "@Style"; break;
        case OriginType::JavaScript: ss << "@JavaScript"; break;
        case OriginType::Custom: ss << "@" << typeName_; break;
    }
    ss << ")";
    return ss.str();
}

SharedPtr<BaseNode> OriginNode::clone() const {
    auto node = std::make_shared<OriginNode>(originType_, content_, range_);
    node->typeName_ = typeName_;
    return node;
}

// ImportNode实现
ImportNode::ImportNode(const String& path, const SourceRange& range)
    : BaseNode(NodeType::Import, range), path_(path) {}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ImportNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Import(" << path_;
    if (!alias_.empty()) {
        ss << " as " << alias_;
    }
    ss << ")";
    return ss.str();
}

SharedPtr<BaseNode> ImportNode::clone() const {
    auto node = std::make_shared<ImportNode>(path_, range_);
    node->alias_ = alias_;
    return node;
}

} // namespace CHTL
