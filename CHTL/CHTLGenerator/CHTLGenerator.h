#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "CHTL/CHTLNode/BaseNode.h"

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();

    // 生成HTML代码
    std::string generateHTML(std::shared_ptr<BaseNode> ast);
    
    // 生成CSS代码
    std::string generateCSS(std::shared_ptr<BaseNode> ast);
    
    // 生成JavaScript代码
    std::string generateJavaScript(std::shared_ptr<BaseNode> ast);
    
    // 生成完整页面
    std::string generatePage(std::shared_ptr<BaseNode> ast);
    
    // 设置配置选项
    void setInlineCSS(bool inlineCSS);
    void setInlineJS(bool inlineJS);
    void setDefaultStructure(bool defaultStructure);
    
    // 获取生成的代码
    const std::string& getHTML() const;
    const std::string& getCSS() const;
    const std::string& getJavaScript() const;

private:
    std::string html_;
    std::string css_;
    std::string javascript_;
    
    bool inlineCSS_;
    bool inlineJS_;
    bool defaultStructure_;
    
    // HTML生成方法
    std::string generateElement(std::shared_ptr<BaseNode> node);
    std::string generateTextNode(std::shared_ptr<BaseNode> node);
    std::string generateComment(std::shared_ptr<BaseNode> node);
    std::string generateStyleBlock(std::shared_ptr<BaseNode> node);
    std::string generateScriptBlock(std::shared_ptr<BaseNode> node);
    std::string generateTemplate(std::shared_ptr<BaseNode> node);
    std::string generateCustom(std::shared_ptr<BaseNode> node);
    std::string generateOrigin(std::shared_ptr<BaseNode> node);
    std::string generateImport(std::shared_ptr<BaseNode> node);
    std::string generateNamespace(std::shared_ptr<BaseNode> node);
    std::string generateConfiguration(std::shared_ptr<BaseNode> node);
    std::string generateUse(std::shared_ptr<BaseNode> node);
    
    // CSS生成方法
    std::string generateStyleProperties(std::shared_ptr<BaseNode> styleNode);
    std::string generateStyleSelector(const std::string& selector);
    std::string generateStyleRule(const std::string& selector, const std::unordered_map<std::string, std::string>& properties);
    
    // JavaScript生成方法
    std::string generateScriptContent(std::shared_ptr<BaseNode> scriptNode);
    
    // 辅助方法
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJavaScript(const std::string& text);
    
    std::string formatAttributes(const std::unordered_map<std::string, std::string>& attributes);
    std::string formatStyleProperties(const std::unordered_map<std::string, std::string>& properties);
    
    // 处理特殊节点
    void processTemplate(std::shared_ptr<BaseNode> templateNode);
    void processCustom(std::shared_ptr<BaseNode> customNode);
    void processOrigin(std::shared_ptr<BaseNode> originNode);
    void processImport(std::shared_ptr<BaseNode> importNode);
    void processNamespace(std::shared_ptr<BaseNode> namespaceNode);
    void processConfiguration(std::shared_ptr<BaseNode> configNode);
    void processUse(std::shared_ptr<BaseNode> useNode);
    
    // 样式处理
    void processStyleBlock(std::shared_ptr<BaseNode> styleNode);
    void processLocalStyle(std::shared_ptr<BaseNode> styleNode, std::shared_ptr<BaseNode> parentElement);
    void processGlobalStyle(std::shared_ptr<BaseNode> styleNode);
    
    // 局部样式块处理
    void processLocalStyleBlock(std::shared_ptr<BaseNode> styleNode, std::shared_ptr<BaseNode> parentElement);
    void processStyleSelector(std::shared_ptr<BaseNode> styleNode, std::shared_ptr<BaseNode> parentElement);
    void processStylePropertyWithOperations(std::shared_ptr<BaseNode> styleNode);
    std::string processPropertyValue(const std::string& value);
    std::string processPropertyExpression(const std::string& expression);
    std::string processPropertyReference(const std::string& reference);
    std::string processPropertyConditional(const std::string& conditional);
    std::string processPropertyArithmetic(const std::string& arithmetic);
    
    // 选择器处理
    std::string processClassSelector(const std::string& selector, std::shared_ptr<BaseNode> parentElement);
    std::string processIdSelector(const std::string& selector, std::shared_ptr<BaseNode> parentElement);
    std::string processPseudoSelector(const std::string& selector);
    std::string processContextSelector(const std::string& selector, std::shared_ptr<BaseNode> parentElement);
    
    // 属性运算处理
    std::string processArithmeticExpression(const std::string& expression);
    std::string processArithmeticTerm(const std::string& term);
    std::string processArithmeticFactor(const std::string& factor);
    std::string processArithmeticOperator(const std::string& op);
    
    // 属性条件表达式处理
    std::string processConditionalExpression(const std::string& expression);
    std::string processConditionalTerm(const std::string& term);
    std::string processConditionalFactor(const std::string& factor);
    std::string processConditionalOperator(const std::string& op);
    
    // 引用属性处理
    std::string processReferenceSelector(const std::string& selector);
    std::string processReferenceProperty(const std::string& property);
    
    // 脚本处理
    void processScriptBlock(std::shared_ptr<BaseNode> scriptNode);
    void processLocalScript(std::shared_ptr<BaseNode> scriptNode, std::shared_ptr<BaseNode> parentElement);
    void processGlobalScript(std::shared_ptr<BaseNode> scriptNode);
    
    // 模板和自定义处理
    void processTemplates(std::shared_ptr<BaseNode> ast);
    void processCustoms(std::shared_ptr<BaseNode> ast);
    
    // 导入处理
    void processImports(std::shared_ptr<BaseNode> ast);
    
    // 命名空间处理
    void processNamespaces(std::shared_ptr<BaseNode> ast);
    
    // 配置处理
    void processConfigurations(std::shared_ptr<BaseNode> ast);
    
    // 使用处理
    void processUses(std::shared_ptr<BaseNode> ast);
    
    // 生成默认结构
    std::string generateDefaultStructure();
    
    // 生成页面头部
    std::string generatePageHeader();
    
    // 生成页面尾部
    std::string generatePageFooter();
    
    // 清理和优化
    void optimizeHTML();
    void optimizeCSS();
    void optimizeJavaScript();
    
    // 验证生成的代码
    bool validateHTML(const std::string& html);
    bool validateCSS(const std::string& css);
    bool validateJavaScript(const std::string& javascript);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H