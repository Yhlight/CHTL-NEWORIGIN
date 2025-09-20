#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <memory>
#include "CHTLNode/CHTLNode.h"

class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;
    
    // 生成HTML代码
    bool generate(const std::unique_ptr<CHTLNode>& ast, std::string& output);
    
    // 设置生成选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setIndentSize(int size) { indentSize_ = size; }
    void setMinify(bool enabled) { minify_ = enabled; }
    
private:
    // 生成选项
    bool debugMode_;
    int indentSize_;
    bool minify_;
    
    // 生成状态
    std::string output_;
    int currentIndent_;
    
    // 主要生成方法
    void generateNode(const CHTLNode& node);
    void generateElement(const ElementNode& node);
    void generateText(const TextNode& node);
    void generateTemplate(const TemplateNode& node);
    void generateCustom(const CustomNode& node);
    void generateStyle(const StyleNode& node);
    void generateScript(const ScriptNode& node);
    void generateExpression(const ExpressionNode& node);
    void generateConditional(const ConditionalNode& node);
    void generateOperation(const OperationNode& node);
    
    // 辅助方法
    void addIndent();
    void addLine();
    void addText(const std::string& text);
    void addAttribute(const std::string& name, const std::string& value);
    std::string escapeHtml(const std::string& text);
    std::string escapeAttribute(const std::string& text);
    
    // 错误处理
    void reportError(const std::string& message);
    
    // 生成状态
    std::vector<std::string> errors_;
};

#endif // CHTL_GENERATOR_H