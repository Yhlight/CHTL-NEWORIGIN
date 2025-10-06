#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include "../../Common.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// 节点类型
enum class NodeType {
    Unknown,
    Program,         // 根节点
    Element,         // 元素节点
    Text,            // 文本节点
    Comment,         // 注释节点
    Attribute,       // 属性节点
    Style,           // 样式节点
    Script,          // 脚本节点
    Template,        // 模板节点
    Custom,          // 自定义节点
    Origin,          // 原始嵌入节点
    Import,          // 导入节点
    Namespace,       // 命名空间节点
    Configuration,   // 配置节点
    Expression,      // 表达式节点
    Selector,        // 选择器节点
};

// AST节点基类
class BaseNode {
public:
    BaseNode(NodeType type, const SourceRange& range = SourceRange());
    virtual ~BaseNode() = default;
    
    // 获取节点类型
    NodeType getType() const { return type_; }
    
    // 获取源代码位置
    const SourceRange& getRange() const { return range_; }
    void setRange(const SourceRange& range) { range_ = range; }
    
    // 子节点管理
    void addChild(SharedPtr<BaseNode> child);
    const Vector<SharedPtr<BaseNode>>& getChildren() const { return children_; }
    Vector<SharedPtr<BaseNode>>& getChildren() { return children_; }
    
    // 父节点
    void setParent(BaseNode* parent) { parent_ = parent; }
    BaseNode* getParent() const { return parent_; }
    
    // 访问者模式
    virtual void accept(class NodeVisitor& visitor) = 0;
    
    // 转换为字符串（调试用）
    virtual String toString(int indent = 0) const;
    
    // 克隆节点
    virtual SharedPtr<BaseNode> clone() const = 0;
    
protected:
    NodeType type_;
    SourceRange range_;
    Vector<SharedPtr<BaseNode>> children_;
    BaseNode* parent_ = nullptr;
    
    // 辅助方法
    String indentString(int indent) const;
};

// 程序根节点
class ProgramNode : public BaseNode {
public:
    ProgramNode();
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
};

// 元素节点
class ElementNode : public BaseNode {
public:
    ElementNode(const String& tagName, const SourceRange& range = SourceRange());
    
    const String& getTagName() const { return tagName_; }
    void setTagName(const String& name) { tagName_ = name; }
    
    // 属性管理
    void setAttribute(const String& name, const String& value);
    Optional<String> getAttribute(const String& name) const;
    const HashMap<String, String>& getAttributes() const { return attributes_; }
    bool hasAttribute(const String& name) const;
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    String tagName_;
    HashMap<String, String> attributes_;
};

// 文本节点
class TextNode : public BaseNode {
public:
    TextNode(const String& content, const SourceRange& range = SourceRange());
    
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    String content_;
};

// 注释节点
class CommentNode : public BaseNode {
public:
    enum class CommentType {
        Line,        // //
        Block,       // /* */
        Generator,   // #
    };
    
    CommentNode(const String& content, CommentType commentType, 
                const SourceRange& range = SourceRange());
    
    const String& getContent() const { return content_; }
    CommentType getCommentType() const { return commentType_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    String content_;
    CommentType commentType_;
};

// 属性节点
class AttributeNode : public BaseNode {
public:
    AttributeNode(const String& name, const String& value, 
                  const SourceRange& range = SourceRange());
    
    const String& getName() const { return name_; }
    const String& getValue() const { return value_; }
    
    void setName(const String& name) { name_ = name; }
    void setValue(const String& value) { value_ = value; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    String name_;
    String value_;
};

// 样式节点
class StyleNode : public BaseNode {
public:
    StyleNode(const SourceRange& range = SourceRange());
    
    // 内联样式
    void addInlineStyle(const String& property, const String& value);
    const HashMap<String, String>& getInlineStyles() const { return inlineStyles_; }
    
    // CSS规则
    void addCssRule(const String& selector, const String& rules);
    const HashMap<String, String>& getCssRules() const { return cssRules_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    HashMap<String, String> inlineStyles_;  // 内联样式
    HashMap<String, String> cssRules_;      // CSS规则
};

// 脚本节点
class ScriptNode : public BaseNode {
public:
    ScriptNode(const String& content, const SourceRange& range = SourceRange());
    
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    String content_;
    bool isLocal_ = false;  // 是否是局部script块
};

// 模板节点
class TemplateNode : public BaseNode {
public:
    enum class TemplateType {
        Style,      // @Style
        Element,    // @Element
        Var,        // @Var
    };
    
    TemplateNode(TemplateType templateType, const String& name,
                 const SourceRange& range = SourceRange());
    
    TemplateType getTemplateType() const { return templateType_; }
    const String& getName() const { return name_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    TemplateType templateType_;
    String name_;
};

// 自定义节点
class CustomNode : public BaseNode {
public:
    enum class CustomType {
        Style,      // @Style
        Element,    // @Element
        Var,        // @Var
    };
    
    CustomNode(CustomType customType, const String& name,
               const SourceRange& range = SourceRange());
    
    CustomType getCustomType() const { return customType_; }
    const String& getName() const { return name_; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    CustomType customType_;
    String name_;
};

// 原始嵌入节点
class OriginNode : public BaseNode {
public:
    enum class OriginType {
        Html,
        Style,
        JavaScript,
        Custom,  // 自定义类型
    };
    
    OriginNode(OriginType originType, const String& content,
               const SourceRange& range = SourceRange());
    
    OriginType getOriginType() const { return originType_; }
    const String& getContent() const { return content_; }
    const String& getTypeName() const { return typeName_; }
    
    void setTypeName(const String& name) { typeName_ = name; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    OriginType originType_;
    String content_;
    String typeName_;  // 对于自定义类型，存储类型名称
};

// 导入节点
class ImportNode : public BaseNode {
public:
    ImportNode(const String& path, const SourceRange& range = SourceRange());
    
    const String& getPath() const { return path_; }
    const String& getAlias() const { return alias_; }
    
    void setAlias(const String& alias) { alias_ = alias; }
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    String path_;
    String alias_;
};

// 访问者接口
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
