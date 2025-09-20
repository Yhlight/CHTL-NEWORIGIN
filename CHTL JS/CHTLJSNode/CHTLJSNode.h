#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "CHTLJSLexer/CHTLJSLexer.h"

// CHTL JS节点类型枚举
enum class CHTLJSNodeType {
    // 基础节点
    ROOT,           // 根节点
    EXPRESSION,     // 表达式节点
    STATEMENT,      // 语句节点
    
    // 函数节点
    LISTEN,         // Listen函数
    ANIMATE,        // Animate函数
    ROUTER,         // Router函数
    VIR,            // Vir虚对象
    INEVERAWAY,     // iNeverAway函数
    PRINTMYLOVE,    // printMylove函数
    SCRIPTLOADER,   // ScriptLoader函数
    
    // 选择器节点
    SELECTOR,       // 选择器节点
    ENHANCED_SELECTOR, // 增强选择器节点
    
    // 事件节点
    EVENT_BINDING,  // 事件绑定节点
    EVENT_DELEGATE, // 事件委托节点
    
    // 动画节点
    ANIMATION,      // 动画节点
    ANIMATION_KEYFRAME, // 动画关键帧节点
    
    // 路由节点
    ROUTE,          // 路由节点
    
    // 虚对象节点
    VIRTUAL_OBJECT, // 虚对象节点
    
    // 其他
    UNKNOWN         // 未知节点
};

// 基础CHTL JS节点类
class CHTLJSNode {
public:
    CHTLJSNode(CHTLJSNodeType type, const std::string& value = "");
    virtual ~CHTLJSNode() = default;
    
    // 基本属性
    CHTLJSNodeType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    void setValue(const std::string& value) { value_ = value; }
    
    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 子节点管理
    void addChild(std::unique_ptr<CHTLJSNode> child);
    const std::vector<std::unique_ptr<CHTLJSNode>>& getChildren() const { return children_; }
    std::vector<std::unique_ptr<CHTLJSNode>>& getChildren() { return children_; }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    const std::string& getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    const std::map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // 访问者模式
    virtual void accept(class CHTLJSNodeVisitor& visitor);
    
    // 调试信息
    virtual std::string toString() const;
    virtual std::string toDebugString(int indent = 0) const;
    
protected:
    CHTLJSNodeType type_;
    std::string value_;
    size_t line_;
    size_t column_;
    std::vector<std::unique_ptr<CHTLJSNode>> children_;
    std::map<std::string, std::string> attributes_;
};

// Listen节点
class ListenNode : public CHTLJSNode {
public:
    ListenNode();
    virtual ~ListenNode() = default;
    
    virtual std::string toString() const override;
};

// Animate节点
class AnimateNode : public CHTLJSNode {
public:
    AnimateNode();
    virtual ~AnimateNode() = default;
    
    virtual std::string toString() const override;
};

// Router节点
class RouterNode : public CHTLJSNode {
public:
    RouterNode();
    virtual ~RouterNode() = default;
    
    virtual std::string toString() const override;
};

// 选择器节点
class SelectorNode : public CHTLJSNode {
public:
    SelectorNode(const std::string& selector);
    virtual ~SelectorNode() = default;
    
    const std::string& getSelector() const { return selector_; }
    void setSelector(const std::string& selector) { selector_ = selector; }
    
    virtual std::string toString() const override;
    
private:
    std::string selector_;
};

// 事件绑定节点
class EventBindingNode : public CHTLJSNode {
public:
    EventBindingNode(const std::string& eventType, const std::string& handler);
    virtual ~EventBindingNode() = default;
    
    const std::string& getEventType() const { return eventType_; }
    const std::string& getHandler() const { return handler_; }
    
    virtual std::string toString() const override;
    
private:
    std::string eventType_;
    std::string handler_;
};

// 动画节点
class AnimationNode : public CHTLJSNode {
public:
    AnimationNode();
    virtual ~AnimationNode() = default;
    
    virtual std::string toString() const override;
};

// 路由节点
class RouteNode : public CHTLJSNode {
public:
    RouteNode(const std::string& url, const std::string& page);
    virtual ~RouteNode() = default;
    
    const std::string& getUrl() const { return url_; }
    const std::string& getPage() const { return page_; }
    
    virtual std::string toString() const override;
    
private:
    std::string url_;
    std::string page_;
};

// 虚对象节点
class VirtualObjectNode : public CHTLJSNode {
public:
    VirtualObjectNode(const std::string& name);
    virtual ~VirtualObjectNode() = default;
    
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    virtual std::string toString() const override;
    
private:
    std::string name_;
};

// CHTL JS节点访问者接口
class CHTLJSNodeVisitor {
public:
    virtual ~CHTLJSNodeVisitor() = default;
    virtual void visit(CHTLJSNode& node) = 0;
    virtual void visit(ListenNode& node) = 0;
    virtual void visit(AnimateNode& node) = 0;
    virtual void visit(RouterNode& node) = 0;
    virtual void visit(SelectorNode& node) = 0;
    virtual void visit(EventBindingNode& node) = 0;
    virtual void visit(AnimationNode& node) = 0;
    virtual void visit(RouteNode& node) = 0;
    virtual void visit(VirtualObjectNode& node) = 0;
};

#endif // CHTL_JS_NODE_H