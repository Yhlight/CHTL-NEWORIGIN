#pragma once

#include "../Common.hpp"
#include "Token.hpp"

namespace CHTL {

// 前向声明
class NodeVisitor;

// 节点类型
enum class NodeType {
    // 基础节点
    Element,           // 元素节点 (div, span等)
    Text,              // 文本节点
    Comment,           // 注释节点
    
    // 样式和脚本
    Style,             // 样式节点
    Script,            // 脚本节点
    LocalStyle,        // 局部样式块
    LocalScript,       // 局部脚本块
    
    // 模板和自定义
    Template,          // 模板节点
    Custom,            // 自定义节点
    TemplateStyle,     // 样式组模板
    TemplateElement,   // 元素模板
    TemplateVar,       // 变量组模板
    CustomStyle,       // 自定义样式组
    CustomElement,     // 自定义元素
    CustomVar,         // 自定义变量组
    
    // 原始嵌入和导入
    Origin,            // 原始嵌入
    Import,            // 导入
    
    // 配置和命名空间
    Configuration,     // 配置组
    Namespace,         // 命名空间
    
    // 属性和表达式
    Attribute,         // 属性
    Property,          // CSS属性
    Expression,        // 表达式
    ConditionalExpr,   // 条件表达式
    BinaryExpr,        // 二元表达式
    UnaryExpr,         // 一元表达式
    
    // 操作符节点
    Delete,            // delete操作
    Insert,            // insert操作
    Inherit,           // inherit操作
    
    // 其他
    Selector,          // 选择器
    Reference,         // 引用
    If,                // if条件
    ElseIf,            // else if条件
    Else               // else
};

// 基础节点类
class Node {
public:
    explicit Node(NodeType type) : type_(type) {}
    virtual ~Node() = default;
    
    NodeType type() const { return type_; }
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) = 0;
    
    // 获取节点的字符串表示（用于调试）
    virtual String toString() const = 0;
    
    // 添加子节点
    void addChild(Ptr<Node> child) {
        children_.push_back(std::move(child));
    }
    
    const Vec<Ptr<Node>>& children() const { return children_; }
    Vec<Ptr<Node>>& children() { return children_; }
    
protected:
    NodeType type_;
    Vec<Ptr<Node>> children_;
};

// 元素节点
class ElementNode : public Node {
public:
    ElementNode(String tagName)
        : Node(NodeType::Element), tagName_(std::move(tagName)) {}
    
    const String& tagName() const { return tagName_; }
    void setTagName(const String& name) { tagName_ = name; }
    
    void addAttribute(const String& name, const String& value) {
        attributes_[name] = value;
    }
    
    const Map<String, String>& attributes() const { return attributes_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Element(" + tagName_ + ")";
    }
    
private:
    String tagName_;
    Map<String, String> attributes_;
};

// 文本节点
class TextNode : public Node {
public:
    explicit TextNode(String content)
        : Node(NodeType::Text), content_(std::move(content)) {}
    
    const String& content() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Text(\"" + content_ + "\")";
    }
    
private:
    String content_;
};

// 注释节点
class CommentNode : public Node {
public:
    explicit CommentNode(String content, bool isGenerator = false)
        : Node(NodeType::Comment), content_(std::move(content)), 
          isGenerator_(isGenerator) {}
    
    const String& content() const { return content_; }
    bool isGenerator() const { return isGenerator_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return (isGenerator_ ? "GeneratorComment" : "Comment") + 
               "(\"" + content_ + "\")";
    }
    
private:
    String content_;
    bool isGenerator_;
};

// 样式节点
class StyleNode : public Node {
public:
    StyleNode() : Node(NodeType::Style) {}
    
    void addProperty(const String& name, const String& value) {
        properties_[name] = value;
    }
    
    const Map<String, String>& properties() const { return properties_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Style(" + std::to_string(properties_.size()) + " properties)";
    }
    
private:
    Map<String, String> properties_;
};

// 脚本节点
class ScriptNode : public Node {
public:
    explicit ScriptNode(String content)
        : Node(NodeType::Script), content_(std::move(content)) {}
    
    const String& content() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Script";
    }
    
private:
    String content_;
};

// 模板节点基类
class TemplateNode : public Node {
public:
    TemplateNode(NodeType type, String name)
        : Node(type), name_(std::move(name)) {}
    
    const String& name() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Template(" + name_ + ")";
    }
    
private:
    String name_;
};

// 自定义节点基类
class CustomNode : public Node {
public:
    CustomNode(NodeType type, String name)
        : Node(type), name_(std::move(name)) {}
    
    const String& name() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Custom(" + name_ + ")";
    }
    
private:
    String name_;
};

// 原始嵌入节点
class OriginNode : public Node {
public:
    OriginNode(String type, String content, Opt<String> name = std::nullopt)
        : Node(NodeType::Origin), originType_(std::move(type)), 
          content_(std::move(content)), name_(std::move(name)) {}
    
    const String& originType() const { return originType_; }
    const String& content() const { return content_; }
    const Opt<String>& name() const { return name_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        String result = "Origin(" + originType_;
        if (name_) {
            result += ", " + *name_;
        }
        result += ")";
        return result;
    }
    
private:
    String originType_;
    String content_;
    Opt<String> name_;
};

// 导入节点
class ImportNode : public Node {
public:
    ImportNode(String type, String from, Opt<String> as = std::nullopt)
        : Node(NodeType::Import), importType_(std::move(type)), 
          from_(std::move(from)), as_(std::move(as)) {}
    
    const String& importType() const { return importType_; }
    const String& from() const { return from_; }
    const Opt<String>& as() const { return as_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        String result = "Import(" + importType_ + " from " + from_;
        if (as_) {
            result += " as " + *as_;
        }
        result += ")";
        return result;
    }
    
private:
    String importType_;
    String from_;
    Opt<String> as_;
};

// 命名空间节点
class NamespaceNode : public Node {
public:
    explicit NamespaceNode(String name)
        : Node(NodeType::Namespace), name_(std::move(name)) {}
    
    const String& name() const { return name_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Namespace(" + name_ + ")";
    }
    
private:
    String name_;
};

// 配置节点
class ConfigurationNode : public Node {
public:
    ConfigurationNode(Opt<String> name = std::nullopt)
        : Node(NodeType::Configuration), name_(std::move(name)) {}
    
    const Opt<String>& name() const { return name_; }
    
    void addSetting(const String& key, const String& value) {
        settings_[key] = value;
    }
    
    const Map<String, String>& settings() const { return settings_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        String result = "Configuration";
        if (name_) {
            result += "(" + *name_ + ")";
        }
        return result;
    }
    
private:
    Opt<String> name_;
    Map<String, String> settings_;
};

// 表达式节点
class ExpressionNode : public Node {
public:
    explicit ExpressionNode(String expr)
        : Node(NodeType::Expression), expression_(std::move(expr)) {}
    
    const String& expression() const { return expression_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Expression(\"" + expression_ + "\")";
    }
    
private:
    String expression_;
};

// 属性节点
class AttributeNode : public Node {
public:
    AttributeNode(String name, String value)
        : Node(NodeType::Attribute), name_(std::move(name)), 
          value_(std::move(value)) {}
    
    const String& name() const { return name_; }
    const String& value() const { return value_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString() const override {
        return "Attribute(" + name_ + " = \"" + value_ + "\")";
    }
    
private:
    String name_;
    String value_;
};

// 访问者接口
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visitElement(ElementNode& node) = 0;
    virtual void visitText(TextNode& node) = 0;
    virtual void visitComment(CommentNode& node) = 0;
    virtual void visitStyle(StyleNode& node) = 0;
    virtual void visitScript(ScriptNode& node) = 0;
    virtual void visitTemplate(TemplateNode& node) = 0;
    virtual void visitCustom(CustomNode& node) = 0;
    virtual void visitOrigin(OriginNode& node) = 0;
    virtual void visitImport(ImportNode& node) = 0;
    virtual void visitNamespace(NamespaceNode& node) = 0;
    virtual void visitConfiguration(ConfigurationNode& node) = 0;
    virtual void visitExpression(ExpressionNode& node) = 0;
    virtual void visitAttribute(AttributeNode& node) = 0;
};

// 实现accept方法
inline void ElementNode::accept(NodeVisitor& visitor) { visitor.visitElement(*this); }
inline void TextNode::accept(NodeVisitor& visitor) { visitor.visitText(*this); }
inline void CommentNode::accept(NodeVisitor& visitor) { visitor.visitComment(*this); }
inline void StyleNode::accept(NodeVisitor& visitor) { visitor.visitStyle(*this); }
inline void ScriptNode::accept(NodeVisitor& visitor) { visitor.visitScript(*this); }
inline void TemplateNode::accept(NodeVisitor& visitor) { visitor.visitTemplate(*this); }
inline void CustomNode::accept(NodeVisitor& visitor) { visitor.visitCustom(*this); }
inline void OriginNode::accept(NodeVisitor& visitor) { visitor.visitOrigin(*this); }
inline void ImportNode::accept(NodeVisitor& visitor) { visitor.visitImport(*this); }
inline void NamespaceNode::accept(NodeVisitor& visitor) { visitor.visitNamespace(*this); }
inline void ConfigurationNode::accept(NodeVisitor& visitor) { visitor.visitConfiguration(*this); }
inline void ExpressionNode::accept(NodeVisitor& visitor) { visitor.visitExpression(*this); }
inline void AttributeNode::accept(NodeVisitor& visitor) { visitor.visitAttribute(*this); }

} // namespace CHTL
