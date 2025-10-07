#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include "../../Common.h"
#include "../CHTLLexer/Token.h"
#include "nodes/NodeVisitor.h"
#include <memory>

namespace CHTL {

// Forward declaration of the visitor
class NodeVisitor;

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
class BaseNode : public std::enable_shared_from_this<BaseNode> {
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
    void setParent(SharedPtr<BaseNode> parent) { parent_ = parent; }
    SharedPtr<BaseNode> getParent() const { return parent_.lock(); }
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) = 0;
    
    // 转换为字符串（调试用）
    virtual String toString(int indent = 0) const;
    
    // 克隆节点
    virtual SharedPtr<BaseNode> clone() const = 0;
    
protected:
    NodeType type_;
    SourceRange range_;
    Vector<SharedPtr<BaseNode>> children_;
    std::weak_ptr<BaseNode> parent_;
    
    // 辅助方法
    String indentString(int indent) const;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
