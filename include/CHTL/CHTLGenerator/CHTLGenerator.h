#pragma once

#include "CHTLCommon.h"
#include "ASTNode.h"
#include <memory>
#include <string>
#include <map>

namespace CHTL {

// 代码生成结果
struct GenerationResult {
    std::string html;
    std::string css;
    std::string js;
    bool success;
    std::vector<std::string> errors;
    
    GenerationResult() : success(false) {}
};

// CHTL Generator - 将AST转换为HTML/CSS/JS代码
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();

    // 生成代码
    GenerationResult generate(const std::shared_ptr<ASTNode>& ast);
    std::string generateHTML(const std::shared_ptr<ASTNode>& ast);
    std::string generateCSS(const std::shared_ptr<ASTNode>& ast);
    std::string generateJS(const std::shared_ptr<ASTNode>& ast);

    // 配置选项
    void setIndentSize(size_t size) { indentSize = size; }
    void setUseSpaces(bool spaces) { useSpaces = spaces; }
    void setMinifyOutput(bool minify) { minifyOutput = minify; }
    void setIncludeComments(bool include) { includeComments = include; }
    void setDefaultHTMLStructure(bool enable) { defaultHTMLStructure = enable; }
    void setDebugMode(bool debug) { debugMode = debug; }

    // 模板管理
    void setHTMLTemplate(const std::string& template_) { htmlTemplate = template_; }
    void setCSSTemplate(const std::string& template_) { cssTemplate = template_; }
    void setJSTemplate(const std::string& template_) { jsTemplate = template_; }

    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    // HTML生成方法
    std::string generateElement(const std::shared_ptr<ASTNode>& node);
    std::string generateAttributes(const std::shared_ptr<ASTNode>& node);
    std::string generateAttribute(const std::shared_ptr<ASTNode>& node);
    std::string generateTextNode(const std::shared_ptr<ASTNode>& node);
    std::string generateComment(const std::shared_ptr<ASTNode>& node);
    std::string generateGeneratorComment(const std::shared_ptr<ASTNode>& node);

    // CSS生成方法
    std::string generateLocalStyle(const std::shared_ptr<ASTNode>& node);
    std::string generateStyleProperty(const std::shared_ptr<ASTNode>& node);
    std::string generateGlobalStyle(const std::shared_ptr<ASTNode>& node);

    // JS生成方法
    std::string generateScript(const std::shared_ptr<ASTNode>& node);
    std::string generateCHTLJSCode(const std::string& source);
    
    // 递归生成方法
    void generateCSSRecursive(const std::shared_ptr<ASTNode>& node, std::stringstream& css);
    void generateJSRecursive(const std::shared_ptr<ASTNode>& node, std::stringstream& js);
    
    // 辅助方法
    bool isSelfClosingTag(const std::string& tagName);

    // 辅助方法
    std::string getIndent(size_t level);
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJS(const std::string& text);
    std::string generateCSSSelector(const std::shared_ptr<ASTNode>& element);
    std::string generateUniqueID(const std::string& prefix = "chtl");
    
    // 错误报告
    void error(const std::string& message);

    // 状态管理
    std::vector<std::string> errors;
    std::function<void(const std::string&)> errorHandler;
    
    // 配置
    size_t indentSize;
    bool useSpaces;
    bool minifyOutput;
    bool includeComments;
    bool defaultHTMLStructure;
    bool debugMode;
    
    // 模板
    std::string htmlTemplate;
    std::string cssTemplate;
    std::string jsTemplate;
    
    // 生成状态
    size_t currentIndent;
    std::map<std::string, std::string> generatedIDs;
    std::vector<std::string> globalStyles;
    std::vector<std::string> globalScripts;
    
    // 调试辅助
    void debugPrint(const std::string& message);
};

// 生成器工具函数
namespace GeneratorUtils {
    // 验证生成结果
    bool validateHTML(const std::string& html);
    bool validateCSS(const std::string& css);
    bool validateJS(const std::string& js);
    
    // 格式化代码
    std::string formatHTML(const std::string& html, size_t indentSize = 2);
    std::string formatCSS(const std::string& css, size_t indentSize = 2);
    std::string formatJS(const std::string& js, size_t indentSize = 2);
    
    // 压缩代码
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    // 内联处理
    std::string inlineCSSIntoHTML(const std::string& html, const std::string& css);
    std::string inlineJSIntoHTML(const std::string& html, const std::string& js);
    
    // 默认模板
    std::string getDefaultHTMLTemplate();
    std::string getDefaultCSSTemplate();
    std::string getDefaultJSTemplate();
}

} // namespace CHTL