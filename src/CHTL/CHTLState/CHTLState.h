#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include "../../Common.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// 前向声明
class CHTLParser;
class BaseNode;

// 解析状态类型
enum class StateType {
    Initial,           // 初始状态
    Element,           // 元素解析状态
    Attribute,         // 属性解析状态
    Text,              // 文本解析状态
    Style,             // 样式块解析状态
    Script,            // 脚本块解析状态
    Template,          // 模板解析状态
    Custom,            // 自定义解析状态
    Origin,            // 原始嵌入解析状态
    Import,            // 导入解析状态
    Namespace,         // 命名空间解析状态
    Configuration,     // 配置解析状态
    Comment,           // 注释解析状态
    Expression,        // 表达式解析状态
    Selector,          // 选择器解析状态
};

// 状态基类
class CHTLState {
public:
    CHTLState(StateType type, CHTLParser* parser);
    virtual ~CHTLState() = default;
    
    // 获取状态类型
    StateType getType() const { return type_; }
    
    // 状态处理方法
    virtual SharedPtr<BaseNode> parse() = 0;
    
    // 状态转换
    virtual SharedPtr<CHTLState> transition(const Token& token);
    
    // 检查是否可以处理该Token
    virtual bool canHandle(const Token& token) const = 0;
    
    // 错误处理
    virtual void handleError(const String& message);
    
protected:
    StateType type_;
    CHTLParser* parser_;
    
    // 辅助方法
    Token getCurrentToken() const;
    Token advance();
    Token peek() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    void expect(TokenType type, const String& message);
};

// 初始状态
class InitialState : public CHTLState {
public:
    InitialState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    SharedPtr<CHTLState> transition(const Token& token) override;
    bool canHandle(const Token& token) const override;
};

// 元素解析状态
class ElementState : public CHTLState {
public:
    ElementState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
    
private:
    SharedPtr<BaseNode> parseElement();
    Vector<SharedPtr<BaseNode>> parseElementBody();
};

// 属性解析状态
class AttributeState : public CHTLState {
public:
    AttributeState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
    
private:
    String parseAttributeValue();
};

// 文本解析状态
class TextState : public CHTLState {
public:
    TextState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
    
private:
    String parseTextContent();
};

// 样式块解析状态
class StyleState : public CHTLState {
public:
    StyleState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
    
private:
    String parseStyleContent();
};

// 脚本块解析状态
class ScriptState : public CHTLState {
public:
    ScriptState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
    
private:
    String parseScriptContent();
};

// 模板解析状态
class TemplateState : public CHTLState {
public:
    TemplateState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
};

// 自定义解析状态
class CustomState : public CHTLState {
public:
    CustomState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
};

// 注释解析状态
class CommentState : public CHTLState {
public:
    CommentState(CHTLParser* parser);
    
    SharedPtr<BaseNode> parse() override;
    bool canHandle(const Token& token) const override;
};

} // namespace CHTL

#endif // CHTL_STATE_H
