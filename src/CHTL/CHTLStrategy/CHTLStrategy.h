#ifndef CHTL_STRATEGY_H
#define CHTL_STRATEGY_H

#include "../../Common.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// 前向声明
class BaseNode;
class CHTLParser;

// 策略类型
enum class StrategyType {
    Parse,          // 解析策略
    Validate,       // 验证策略
    Transform,      // 转换策略
    Generate,       // 生成策略
    Optimize,       // 优化策略
};

// 策略基类
class CHTLStrategy {
public:
    CHTLStrategy(StrategyType type);
    virtual ~CHTLStrategy() = default;
    
    StrategyType getType() const { return type_; }
    
    // 执行策略
    virtual bool execute() = 0;
    
    // 检查策略是否适用
    virtual bool isApplicable() const = 0;
    
protected:
    StrategyType type_;
};

// 解析策略基类
class ParseStrategy : public CHTLStrategy {
public:
    ParseStrategy();
    virtual ~ParseStrategy() = default;
    
    // 解析节点
    virtual SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) = 0;
    
    bool execute() override;
    bool isApplicable() const override;
    
protected:
    CHTLParser* parser_;
    Token currentToken_;
};

// 元素解析策略
class ElementParseStrategy : public ParseStrategy {
public:
    SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) override;
    bool isApplicable() const override;
};

// 文本解析策略
class TextParseStrategy : public ParseStrategy {
public:
    SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) override;
    bool isApplicable() const override;
};

// 属性解析策略
class AttributeParseStrategy : public ParseStrategy {
public:
    SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) override;
    bool isApplicable() const override;
};

// 样式解析策略
class StyleParseStrategy : public ParseStrategy {
public:
    SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) override;
    bool isApplicable() const override;
};

// 脚本解析策略
class ScriptParseStrategy : public ParseStrategy {
public:
    SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) override;
    bool isApplicable() const override;
};

// 模板解析策略
class TemplateParseStrategy : public ParseStrategy {
public:
    SharedPtr<BaseNode> parse(CHTLParser* parser, const Token& token) override;
    bool isApplicable() const override;
};

// 验证策略基类
class ValidateStrategy : public CHTLStrategy {
public:
    ValidateStrategy();
    virtual ~ValidateStrategy() = default;
    
    // 验证节点
    virtual bool validate(const SharedPtr<BaseNode>& node) = 0;
    
    bool execute() override;
    
protected:
    SharedPtr<BaseNode> node_;
};

// 语法验证策略
class SyntaxValidateStrategy : public ValidateStrategy {
public:
    bool validate(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// 语义验证策略
class SemanticValidateStrategy : public ValidateStrategy {
public:
    bool validate(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// 转换策略基类
class TransformStrategy : public CHTLStrategy {
public:
    TransformStrategy();
    virtual ~TransformStrategy() = default;
    
    // 转换节点
    virtual SharedPtr<BaseNode> transform(const SharedPtr<BaseNode>& node) = 0;
    
    bool execute() override;
    
protected:
    SharedPtr<BaseNode> node_;
};

// 模板展开策略
class TemplateExpandStrategy : public TransformStrategy {
public:
    SharedPtr<BaseNode> transform(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// 样式提取策略
class StyleExtractionStrategy : public TransformStrategy {
public:
    SharedPtr<BaseNode> transform(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// 生成策略基类
class GenerateStrategy : public CHTLStrategy {
public:
    GenerateStrategy();
    virtual ~GenerateStrategy() = default;
    
    // 生成代码
    virtual String generate(const SharedPtr<BaseNode>& node) = 0;
    
    bool execute() override;
    
protected:
    SharedPtr<BaseNode> node_;
    String output_;
};

// HTML生成策略
class HtmlGenerateStrategy : public GenerateStrategy {
public:
    String generate(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// CSS生成策略
class CssGenerateStrategy : public GenerateStrategy {
public:
    String generate(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// JavaScript生成策略
class JsGenerateStrategy : public GenerateStrategy {
public:
    String generate(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// 优化策略基类
class OptimizeStrategy : public CHTLStrategy {
public:
    OptimizeStrategy();
    virtual ~OptimizeStrategy() = default;
    
    // 优化节点
    virtual SharedPtr<BaseNode> optimize(const SharedPtr<BaseNode>& node) = 0;
    
    bool execute() override;
    
protected:
    SharedPtr<BaseNode> node_;
};

// 节点合并优化策略
class NodeMergeOptimizeStrategy : public OptimizeStrategy {
public:
    SharedPtr<BaseNode> optimize(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

// 冗余消除优化策略
class RedundancyEliminationStrategy : public OptimizeStrategy {
public:
    SharedPtr<BaseNode> optimize(const SharedPtr<BaseNode>& node) override;
    bool isApplicable() const override;
};

} // namespace CHTL

#endif // CHTL_STRATEGY_H
