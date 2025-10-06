#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../../Common.h"
#include "../CHTLNode/BaseNode.h"
#include "../../CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"

namespace CHTL {

// 生成器配置
struct GeneratorConfig {
    bool prettyPrint = true;           // 美化输出
    int indentSize = 2;                 // 缩进大小
    bool includeComments = false;       // 包含注释
    bool inlineStyles = false;          // 内联样式
    bool inlineScripts = false;         // 内联脚本
    bool minify = false;                // 压缩输出
};

// 代码生成器基类
class CHTLGenerator : public NodeVisitor {
public:
    explicit CHTLGenerator(const GeneratorConfig& config = GeneratorConfig());
    virtual ~CHTLGenerator() = default;
    
    // 生成代码
    String generate(const SharedPtr<BaseNode>& root);
    
    // 获取生成的各部分代码
    const String& getHtml() const { return html_; }
    const String& getCss() const { return css_; }
    const String& getJs() const { return js_; }
    
    // 访问者模式实现
    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(TemplateNode& node) override;
    void visit(CustomNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    
protected:
    GeneratorConfig config_;
    String html_;
    String css_;
    String js_;
    int currentIndent_ = 0;
    
    // 辅助方法
    String indent() const;
    void increaseIndent();
    void decreaseIndent();
    void appendHtml(const String& str);
    void appendCss(const String& str);
    void appendJs(const String& str);
    
    // HTML生成
    void generateElement(ElementNode& node);
    void generateAttributes(const HashMap<String, String>& attributes);
    bool isSelfClosingTag(const String& tagName) const;
    
    // CSS生成
    void generateInlineStyle(const HashMap<String, String>& styles);
    void generateCssRule(const String& selector, const String& rules);
    
    // JS生成
    void generateScript(const String& content);
};

// HTML生成器
class HtmlGenerator {
public:
    static String generate(const SharedPtr<BaseNode>& root, const GeneratorConfig& config = GeneratorConfig());
    
private:
    static void generateNode(const SharedPtr<BaseNode>& node, String& output, int indent, const GeneratorConfig& config);
    static void generateElement(const SharedPtr<ElementNode>& element, String& output, int indent, const GeneratorConfig& config);
    static String indentString(int indent, const GeneratorConfig& config);
};

// CSS生成器
class CssGenerator {
public:
    static String generate(const SharedPtr<BaseNode>& root, const GeneratorConfig& config = GeneratorConfig());
    
private:
    static void collectStyles(const SharedPtr<BaseNode>& node, String& output, const GeneratorConfig& config);
};

// JavaScript生成器
class JsGenerator {
public:
    static String generate(const SharedPtr<BaseNode>& root, const GeneratorConfig& config = GeneratorConfig());
    
private:
    static void collectScripts(const SharedPtr<BaseNode>& node, String& output, const GeneratorConfig& config);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
