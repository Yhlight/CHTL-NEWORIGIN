#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "CHTLLexer/CHTLLexer.h"

// 节点类型枚举
enum class NodeType {
    // 基础节点
    ROOT,           // 根节点
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    
    // 模板节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    ORIGIN,         // 原始嵌入节点
    IMPORT,         // 导入节点
    
    // 配置节点
    CONFIGURATION,  // 配置节点
    NAMESPACE,      // 命名空间节点
    INFO,           // 信息节点
    EXPORT,         // 导出节点
    
    // 样式和脚本节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    
    // 属性节点
    ATTRIBUTE,      // 属性节点
    STYLE_ATTRIBUTE, // 样式属性节点
    
    // 表达式节点
    EXPRESSION,     // 表达式节点
    CONDITIONAL,    // 条件表达式节点
    OPERATION,      // 运算表达式节点
    
    // 其他
    UNKNOWN         // 未知节点
};

// 基础节点类
class CHTLNode {
public:
    CHTLNode(NodeType type, const std::string& value = "");
    virtual ~CHTLNode() = default;
    
    // 基本属性
    NodeType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    void setValue(const std::string& value) { value_ = value; }
    
    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 子节点管理
    void addChild(std::unique_ptr<CHTLNode> child);
    const std::vector<std::unique_ptr<CHTLNode>>& getChildren() const { return children_; }
    std::vector<std::unique_ptr<CHTLNode>>& getChildren() { return children_; }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    const std::string& getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    const std::map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // 访问者模式
    virtual void accept(class CHTLNodeVisitor& visitor);
    
    // 调试信息
    virtual std::string toString() const;
    virtual std::string toDebugString(int indent = 0) const;
    
protected:
    NodeType type_;
    std::string value_;
    size_t line_;
    size_t column_;
    std::vector<std::unique_ptr<CHTLNode>> children_;
    std::map<std::string, std::string> attributes_;
};

// 元素节点
class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& tagName);
    virtual ~ElementNode() = default;
    
    const std::string& getTagName() const { return tagName_; }
    void setTagName(const std::string& tagName) { tagName_ = tagName; }
    
    virtual std::string toString() const override;
    
private:
    std::string tagName_;
};

// 文本节点
class TextNode : public CHTLNode {
public:
    TextNode(const std::string& text);
    virtual ~TextNode() = default;
    
    virtual std::string toString() const override;
};

// 模板节点
class TemplateNode : public CHTLNode {
public:
    TemplateNode(const std::string& templateType, const std::string& name);
    virtual ~TemplateNode() = default;
    
    const std::string& getTemplateType() const { return templateType_; }
    const std::string& getTemplateName() const { return templateName_; }
    
    virtual std::string toString() const override;
    
private:
    std::string templateType_;
    std::string templateName_;
};

// 自定义节点
class CustomNode : public CHTLNode {
public:
    CustomNode(const std::string& customType, const std::string& name);
    virtual ~CustomNode() = default;
    
    const std::string& getCustomType() const { return customType_; }
    const std::string& getCustomName() const { return customName_; }
    
    virtual std::string toString() const override;
    
private:
    std::string customType_;
    std::string customName_;
};

// 样式节点
class StyleNode : public CHTLNode {
public:
    StyleNode();
    virtual ~StyleNode() = default;
    
    virtual std::string toString() const override;
};

// 脚本节点
class ScriptNode : public CHTLNode {
public:
    ScriptNode();
    virtual ~ScriptNode() = default;
    
    virtual std::string toString() const override;
};

// 表达式节点
class ExpressionNode : public CHTLNode {
public:
    ExpressionNode(const std::string& expression);
    virtual ~ExpressionNode() = default;
    
    virtual std::string toString() const override;
};

// 条件表达式节点
class ConditionalNode : public CHTLNode {
public:
    ConditionalNode(std::unique_ptr<CHTLNode> condition, 
                   std::unique_ptr<CHTLNode> trueExpr, 
                   std::unique_ptr<CHTLNode> falseExpr);
    virtual ~ConditionalNode() = default;
    
    virtual std::string toString() const override;
};

// 运算表达式节点
class OperationNode : public CHTLNode {
public:
    OperationNode(const std::string& operator_, 
                 std::unique_ptr<CHTLNode> left, 
                 std::unique_ptr<CHTLNode> right);
    virtual ~OperationNode() = default;
    
    const std::string& getOperator() const { return operator_; }
    
    virtual std::string toString() const override;
    
private:
    std::string operator_;
};

// 节点访问者接口
class CHTLNodeVisitor {
public:
    virtual ~CHTLNodeVisitor() = default;
    virtual void visit(CHTLNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(ExpressionNode& node) = 0;
    virtual void visit(ConditionalNode& node) = 0;
    virtual void visit(OperationNode& node) = 0;
};

#endif // CHTL_NODE_H