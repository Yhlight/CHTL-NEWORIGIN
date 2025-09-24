#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

// 节点类型枚举
enum class NodeType {
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR,       // 操作符节点
    UNKNOWN         // 未知节点
};

// 基础节点类
class BaseNode {
public:
    BaseNode(NodeType type);
    virtual ~BaseNode() = default;
    
    // 节点类型
    NodeType getType() const;
    void setType(NodeType type);
    
    // 节点名称
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // 节点值
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    // 父节点
    BaseNode* getParent() const;
    void setParent(BaseNode* parent);
    
    // 子节点
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void removeChild(size_t index);
    void clearChildren();
    
    // 属性
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    void removeAttribute(const std::string& name);
    void clearAttributes();
    
    // 位置信息
    size_t getLine() const;
    void setLine(size_t line);
    size_t getColumn() const;
    void setColumn(size_t column);
    size_t getPosition() const;
    void setPosition(size_t position);
    
    // 节点状态
    bool isVisible() const;
    void setVisible(bool visible);
    bool isProcessed() const;
    void setProcessed(bool processed);
    
    // 节点操作
    virtual std::string toString() const;
    virtual std::string toHTML() const;
    virtual std::string toCSS() const;
    virtual std::string toJS() const;
    
    // 节点查询
    std::vector<std::shared_ptr<BaseNode>> findChildren(NodeType type) const;
    std::vector<std::shared_ptr<BaseNode>> findChildren(const std::string& name) const;
    std::shared_ptr<BaseNode> findFirstChild(NodeType type) const;
    std::shared_ptr<BaseNode> findFirstChild(const std::string& name) const;
    
    // 节点克隆
    virtual std::shared_ptr<BaseNode> clone() const;
    
    // 节点比较
    virtual bool equals(const BaseNode& other) const;
    
    // 节点验证
    virtual bool validate() const;
    
    // 节点优化
    virtual void optimize();
    
    // 节点统计
    size_t getNodeCount() const;
    size_t getDepth() const;
    size_t getChildCount() const;
    
protected:
    NodeType type_;
    std::string name_;
    std::string value_;
    BaseNode* parent_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::unordered_map<std::string, std::string> attributes_;
    size_t line_;
    size_t column_;
    size_t position_;
    bool visible_;
    bool processed_;
    
    // 辅助函数
    std::string escapeString(const std::string& str) const;
    std::string unescapeString(const std::string& str) const;
};

// 节点工厂
class NodeFactory {
public:
    static std::shared_ptr<BaseNode> createNode(NodeType type);
    static std::shared_ptr<BaseNode> createElement(const std::string& name);
    static std::shared_ptr<BaseNode> createText(const std::string& value);
    static std::shared_ptr<BaseNode> createComment(const std::string& value);
    static std::shared_ptr<BaseNode> createTemplate(const std::string& name);
    static std::shared_ptr<BaseNode> createCustom(const std::string& name);
    static std::shared_ptr<BaseNode> createStyle(const std::string& value);
    static std::shared_ptr<BaseNode> createScript(const std::string& value);
    static std::shared_ptr<BaseNode> createOrigin(const std::string& type, const std::string& value);
    static std::shared_ptr<BaseNode> createImport(const std::string& type, const std::string& path);
    static std::shared_ptr<BaseNode> createConfig(const std::string& name);
    static std::shared_ptr<BaseNode> createNamespace(const std::string& name);
    static std::shared_ptr<BaseNode> createOperator(const std::string& op);
};

} // namespace CHTL