#include "CHTLGenerator.h"
#include "../CHTLNode/ConditionalNode.h"
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <cstdio>

namespace CHTL {

// CHTLGenerator实现
CHTLGenerator::CHTLGenerator(const GeneratorConfig& config)
    : config_(config) {}

String CHTLGenerator::generate(const SharedPtr<BaseNode>& root) {
    if (!root) {
        return "";
    }
    
    root->accept(*this);
    
    // 组合最终输出
    std::stringstream result;
    
    // 输出HTML
    result << html_;
    
    // 输出CSS（如果有）
    if (!css_.empty()) {
        if (config_.prettyPrint) {
            result << "\n";
        }
        result << "<style>\n" << css_ << "</style>\n";
    }
    
    // 输出JavaScript（如果有）
    if (!js_.empty()) {
        if (config_.prettyPrint) {
            result << "\n";
        }
        result << "<script>\n" << js_ << "</script>\n";
    }
    
    return result.str();
}

void CHTLGenerator::visit(ProgramNode& node) {
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void CHTLGenerator::visit(ElementNode& node) {
    generateElement(node);
}

void CHTLGenerator::visit(TextNode& node) {
    appendHtml(node.getContent());
}

void CHTLGenerator::visit(CommentNode& node) {
    if (config_.includeComments) {
        switch (node.getCommentType()) {
            case CommentNode::CommentType::Line:
                // HTML中没有单行注释
                appendHtml("<!-- " + node.getContent() + " -->");
                break;
            case CommentNode::CommentType::Block:
            case CommentNode::CommentType::Generator:
                appendHtml("<!-- " + node.getContent() + " -->");
                break;
        }
        if (config_.prettyPrint) {
            appendHtml("\n");
        }
    }
}

void CHTLGenerator::visit(AttributeNode& /* node */) {
    // 属性在元素中处理
}

void CHTLGenerator::visit(StyleNode& node) {
    // 收集CSS规则
    for (const auto& [selector, rules] : node.getCssRules()) {
        generateCssRule(selector, rules);
    }
    
    // 内联样式（如果有的话，在元素中处理）
}

void CHTLGenerator::visit(ScriptNode& node) {
    String content = node.getContent();
    
    // 检查是否包含CHTL JS语法
    if (content.find("{{") != String::npos || 
        content.find("&") != String::npos ||
        content.find("Listen") != String::npos ||
        content.find("Delegate") != String::npos ||
        content.find("Animate") != String::npos ||
        content.find("Router") != String::npos ||
        content.find("ScriptLoader") != String::npos ||
        content.find("Vir") != String::npos ||
        content.find("$") != String::npos) {
        
        JS::JSGeneratorConfig jsConfig;
        jsConfig.wrapIIFE = false;  // 不自动包装IIFE
        jsConfig.prettyPrint = false;
        JS::CHTLJSGenerator jsGen(jsConfig);
        content = jsGen.generate(content);
    }
    
    if (node.isLocal()) {
        // 局部脚本添加到全局脚本区
        appendJs(content);
        appendJs("\n");
    } else {
        generateScript(content);
    }
}

void CHTLGenerator::visit(TemplateNode& /* node */) {
    // 模板在预处理阶段展开，生成时不处理
}

void CHTLGenerator::visit(CustomNode& /* node */) {
    // 自定义在预处理阶段展开，生成时不处理
}

void CHTLGenerator::visit(OriginNode& node) {
    switch (node.getOriginType()) {
        case OriginNode::OriginType::Html:
            appendHtml(node.getContent());
            break;
        case OriginNode::OriginType::Style:
            appendCss(node.getContent());
            break;
        case OriginNode::OriginType::JavaScript:
            appendJs(node.getContent());
            break;
        case OriginNode::OriginType::Custom:
            // 自定义类型按HTML处理
            appendHtml(node.getContent());
            break;
    }
}

void CHTLGenerator::visit(ImportNode& /* node */) {
    // 导入在预处理阶段处理，生成时不处理
}

void CHTLGenerator::visit(ConditionalNode& node) {
    // 处理条件渲染节点
    // 根据CHTL.md规范，条件渲染用于条件性地应用CSS属性
    
    const auto& styles = node.getStyles();
    if (styles.empty()) {
        return;  // 无CSS属性，跳过
    }
    
    String condition = node.getCondition();
    
    if (node.isDynamic()) {
        // 动态条件渲染 - 生成JavaScript代码
        generateDynamicConditional(node);
    } else {
        // 静态条件渲染 - 生成CSS代码
        generateStaticConditional(node);
    }
    
    // 处理else if链
    for (const auto& elseIfNode : node.getElseIfBlocks()) {
        if (elseIfNode) {
            elseIfNode->accept(*this);
        }
    }
    
    // 处理else块
    if (node.getElseBlock()) {
        node.getElseBlock()->accept(*this);
    }
}

void CHTLGenerator::generateElement(ElementNode& node) {
    if (config_.prettyPrint) {
        appendHtml(indent());
    }
    
    // 开始标签
    appendHtml("<" + node.getTagName());
    
    // 属性
    generateAttributes(node.getAttributes());
    
    // 收集内联样式（从StyleNode的子节点）
    HashMap<String, String> inlineStyles;
    for (const auto& child : node.getChildren()) {
        if (child->getType() == NodeType::Style) {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
            const auto& styles = styleNode->getInlineStyles();
            inlineStyles.insert(styles.begin(), styles.end());
        }
    }
    
    // 注册元素到SaltBridge，以便其他元素可以引用其属性
    String elemId = node.hasAttribute("id") ? node.getAttribute("id").value() : "";
    String elemClass = node.hasAttribute("class") ? node.getAttribute("class").value() : "";
    Bridge::SaltBridge::getInstance().registerElementWithProperties(
        node.getTagName(), elemId, elemClass, inlineStyles);
    
    // 生成内联样式
    if (!inlineStyles.empty()) {
        generateInlineStyle(inlineStyles);
    }
    
    // 自闭合标签
    if (isSelfClosingTag(node.getTagName()) && node.getChildren().empty()) {
        appendHtml(" />");
        if (config_.prettyPrint) {
            appendHtml("\n");
        }
        return;
    }
    
    appendHtml(">");
    
    // 推送上下文信息到SaltBridge，以便script块能够正确解析&引用
    Bridge::ContextInfo context;
    context.currentTag = node.getTagName();
    if (node.hasAttribute("id")) {
        context.currentId = node.getAttribute("id").value();
    }
    if (node.hasAttribute("class")) {
        context.currentClass = node.getAttribute("class").value();
    }
    Bridge::SaltBridge::getInstance().pushContext(context);
    
    // 子节点
    bool hasBlockChildren = false;
    for (const auto& child : node.getChildren()) {
        if (child->getType() != NodeType::Text) {
            hasBlockChildren = true;
            break;
        }
    }
    
    if (hasBlockChildren && config_.prettyPrint) {
        appendHtml("\n");
        increaseIndent();
    }
    
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
    
    if (hasBlockChildren && config_.prettyPrint) {
        decreaseIndent();
        appendHtml(indent());
    }
    
    // 弹出上下文
    Bridge::SaltBridge::getInstance().popContext();
    
    // 结束标签
    appendHtml("</" + node.getTagName() + ">");
    if (config_.prettyPrint) {
        appendHtml("\n");
    }
}

void CHTLGenerator::generateAttributes(const HashMap<String, String>& attributes) {
    for (const auto& [name, value] : attributes) {
        appendHtml(" " + name + "=\"" + value + "\"");
    }
}

bool CHTLGenerator::isSelfClosingTag(const String& tagName) const {
    static const std::unordered_set<String> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosingTags.find(tagName) != selfClosingTags.end();
}

void CHTLGenerator::generateInlineStyle(const HashMap<String, String>& styles) {
    if (styles.empty()) {
        return;
    }
    
    appendHtml(" style=\"");
    bool first = true;
    for (const auto& [property, value] : styles) {
        if (!first) {
            appendHtml(" ");
        }
        appendHtml(property + ": " + value + ";");
        first = false;
    }
    appendHtml("\"");
}

void CHTLGenerator::generateCssRule(const String& selector, const String& rules) {
    appendCss(selector + " {\n");
    appendCss("  " + rules + "\n");
    appendCss("}\n\n");
}

void CHTLGenerator::generateScript(const String& content) {
    appendJs(content);
}

void CHTLGenerator::generateStaticConditional(const ConditionalNode& node) {
    // 静态条件渲染：生成CSS注释说明
    // 根据CHTL.md，静态条件使用属性引用（如 html.width < 500px）
    // 实际实现：将条件作为CSS注释，实际应用属性（简化版）
    
    const String& condition = node.getCondition();
    const auto& styles = node.getStyles();
    
    // 添加条件注释到CSS
    appendCss("/* Conditional: " + condition + " */\n");
    
    // TODO: 完整实现应该：
    // 1. 解析条件表达式
    // 2. 根据条件生成 @media 查询或 CSS 变量
    // 3. 对于复杂条件，可能需要生成 JavaScript
    
    // 当前简化实现：直接输出CSS属性作为注释
    for (const auto& [prop, value] : styles) {
        appendCss("  /* " + prop + ": " + value + "; */\n");
    }
    appendCss("\n");
}

void CHTLGenerator::generateDynamicConditional(const ConditionalNode& node) {
    // 动态条件渲染：生成JavaScript代码
    // 根据CHTL.md，动态条件使用{{}}选择器（如 {{html}}->width < 500px）
    
    const String& condition = node.getCondition();
    const auto& styles = node.getStyles();
    
    // 提取{{}}选择器
    auto selectors = node.extractEnhancedSelectors();
    
    // 生成JavaScript监听代码
    appendJs("// Dynamic conditional rendering\n");
    appendJs("(function() {\n");
    appendJs("  function applyConditionalStyles() {\n");
    
    // TODO: 完整实现应该：
    // 1. 解析{{}}选择器，转换为document.querySelector()
    // 2. 解析条件表达式，生成JavaScript表达式
    // 3. 根据条件动态设置style属性
    // 4. 添加resize/mutation等事件监听器
    
    // 当前简化实现：生成条件注释
    appendJs("    // Condition: " + condition + "\n");
    
    for (const auto& [prop, value] : styles) {
        appendJs("    // Apply: " + prop + " = " + value + "\n");
    }
    
    appendJs("  }\n");
    appendJs("  applyConditionalStyles();\n");
    appendJs("  window.addEventListener('resize', applyConditionalStyles);\n");
    appendJs("})();\n\n");
}

String CHTLGenerator::indent() const {
    return String(currentIndent_ * config_.indentSize, ' ');
}

void CHTLGenerator::increaseIndent() {
    currentIndent_++;
}

void CHTLGenerator::decreaseIndent() {
    if (currentIndent_ > 0) {
        currentIndent_--;
    }
}

void CHTLGenerator::appendHtml(const String& str) {
    html_ += str;
}

void CHTLGenerator::appendCss(const String& str) {
    css_ += str;
}

void CHTLGenerator::appendJs(const String& str) {
    js_ += str;
}

// HtmlGenerator实现
String HtmlGenerator::generate(const SharedPtr<BaseNode>& root, const GeneratorConfig& config) {
    String htmlOutput;
    if (root) {
        // 第一遍：收集所有元素和属性到SaltBridge
        collectElements(root);
        
        // 第二遍：生成HTML
        generateNode(root, htmlOutput, 0, config);
    }
    
    // 收集CSS
    String cssOutput = CssGenerator::generate(root, config);
    
    // 收集JavaScript
    String jsOutput = JsGenerator::generate(root, config);
    
    // 组合完整输出
    std::stringstream result;
    result << htmlOutput;
    
    if (!cssOutput.empty()) {
        if (config.prettyPrint) {
            result << "\n";
        }
        result << "<style>\n" << cssOutput << "</style>\n";
    }
    
    if (!jsOutput.empty()) {
        if (config.prettyPrint) {
            result << "\n";
        }
        result << "<script>\n" << jsOutput << "</script>\n";
    }
    
    return result.str();
}

void HtmlGenerator::collectElements(const SharedPtr<BaseNode>& node) {
    if (!node) {
        return;
    }
    
    if (node->getType() == NodeType::Element) {
        auto element = std::dynamic_pointer_cast<ElementNode>(node);
        
        // 收集内联样式
        HashMap<String, String> inlineStyles;
        for (const auto& child : element->getChildren()) {
            if (child->getType() == NodeType::Style) {
                auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
                const auto& styles = styleNode->getInlineStyles();
                inlineStyles.insert(styles.begin(), styles.end());
            }
        }
        
        // 注册到SaltBridge
        String elemId = element->hasAttribute("id") ? element->getAttribute("id").value() : "";
        String elemClass = element->hasAttribute("class") ? element->getAttribute("class").value() : "";
        Bridge::SaltBridge::getInstance().registerElementWithProperties(
            element->getTagName(), elemId, elemClass, inlineStyles);
    }
    
    // 递归处理子节点
    for (const auto& child : node->getChildren()) {
        collectElements(child);
    }
}

void HtmlGenerator::generateNode(const SharedPtr<BaseNode>& node, String& output, int indent, const GeneratorConfig& config) {
    if (!node) {
        return;
    }
    
    switch (node->getType()) {
        case NodeType::Program: {
            auto program = std::dynamic_pointer_cast<ProgramNode>(node);
            for (const auto& child : program->getChildren()) {
                generateNode(child, output, indent, config);
            }
            break;
        }
        
        case NodeType::Element: {
            auto element = std::dynamic_pointer_cast<ElementNode>(node);
            generateElement(element, output, indent, config);
            break;
        }
        
        case NodeType::Text: {
            auto text = std::dynamic_pointer_cast<TextNode>(node);
            output += text->getContent();
            break;
        }
        
        case NodeType::Comment: {
            if (config.includeComments) {
                auto comment = std::dynamic_pointer_cast<CommentNode>(node);
                if (config.prettyPrint) {
                    output += indentString(indent, config);
                }
                output += "<!-- " + comment->getContent() + " -->\n";
            }
            break;
        }
        
        default:
            // 其他节点类型（Style、Script等）不直接生成HTML
            break;
    }
}

void HtmlGenerator::generateElement(const SharedPtr<ElementNode>& element, String& output, int indent, const GeneratorConfig& config) {
    if (config.prettyPrint) {
        output += indentString(indent, config);
    }
    
    // 开始标签
    output += "<" + element->getTagName();
    
    // 属性
    for (const auto& [name, value] : element->getAttributes()) {
        output += " " + name + "=\"" + value + "\"";
    }
    
    // 收集内联样式
    HashMap<String, String> inlineStyles;
    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::Style) {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
            const auto& styles = styleNode->getInlineStyles();
            inlineStyles.insert(styles.begin(), styles.end());
        }
    }
    
    // 生成内联样式
    if (!inlineStyles.empty()) {
        output += " style=\"";
        bool first = true;
        for (const auto& [property, value] : inlineStyles) {
            if (!first) {
                output += " ";
            }
            output += property + ": " + value + ";";
            first = false;
        }
        output += "\"";
    }
    
    // 自闭合标签
    static const std::unordered_set<String> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    if (selfClosingTags.find(element->getTagName()) != selfClosingTags.end() && 
        element->getChildren().empty()) {
        output += " />";
        if (config.prettyPrint) {
            output += "\n";
        }
        return;
    }
    
    output += ">";
    
    // 子节点
    bool hasBlockChildren = false;
    for (const auto& child : element->getChildren()) {
        if (child->getType() != NodeType::Text && child->getType() != NodeType::Style && 
            child->getType() != NodeType::Script) {
            hasBlockChildren = true;
            break;
        }
    }
    
    if (hasBlockChildren && config.prettyPrint) {
        output += "\n";
    }
    
    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::Style || child->getType() == NodeType::Script) {
            continue;  // Style和Script单独处理
        }
        generateNode(child, output, indent + 1, config);
    }
    
    if (hasBlockChildren && config.prettyPrint) {
        output += indentString(indent, config);
    }
    
    // 结束标签
    output += "</" + element->getTagName() + ">";
    if (config.prettyPrint) {
        output += "\n";
    }
}

String HtmlGenerator::indentString(int indent, const GeneratorConfig& config) {
    return String(indent * config.indentSize, ' ');
}

// CssGenerator实现
String CssGenerator::generate(const SharedPtr<BaseNode>& root, const GeneratorConfig& config) {
    String output;
    if (root) {
        collectStyles(root, output, config);
    }
    return output;
}

void CssGenerator::collectStyles(const SharedPtr<BaseNode>& node, String& output, const GeneratorConfig& config) {
    if (!node) {
        return;
    }
    
    if (node->getType() == NodeType::Style) {
        auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
        for (const auto& [selector, rules] : styleNode->getCssRules()) {
            output += selector + " {\n";
            output += "  " + rules + "\n";
            output += "}\n\n";
        }
    }
    
    for (const auto& child : node->getChildren()) {
        collectStyles(child, output, config);
    }
}

// JsGenerator实现
String JsGenerator::generate(const SharedPtr<BaseNode>& root, const GeneratorConfig& config) {
    String output;
    if (root) {
        collectScripts(root, output, config);
    }
    return output;
}

void JsGenerator::collectScripts(const SharedPtr<BaseNode>& node, String& output, const GeneratorConfig& config) {
    if (!node) {
        return;
    }
    
    // 如果是元素节点，推送上下文
    bool pushedContext = false;
    if (node->getType() == NodeType::Element) {
        auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
        Bridge::ContextInfo context;
        context.currentTag = elementNode->getTagName();
        if (elementNode->hasAttribute("id")) {
            context.currentId = elementNode->getAttribute("id").value();
        }
        if (elementNode->hasAttribute("class")) {
            context.currentClass = elementNode->getAttribute("class").value();
        }
        Bridge::SaltBridge::getInstance().pushContext(context);
        pushedContext = true;
    }
    
    if (node->getType() == NodeType::Script) {
        auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(node);
        String content = scriptNode->getContent();
        
        // 检查是否包含CHTL JS语法，如果有则处理
        bool hasCHTLJS = (content.find("{{") != String::npos || 
                         content.find("Listen") != String::npos ||
                         content.find("Delegate") != String::npos ||
                         content.find("Animate") != String::npos ||
                         content.find("Router") != String::npos ||
                         content.find("ScriptLoader") != String::npos ||
                         content.find("Vir") != String::npos ||
                         content.find("&->") != String::npos ||
                         content.find("$") != String::npos);
        
        if (hasCHTLJS) {
            JS::JSGeneratorConfig jsConfig;
            jsConfig.wrapIIFE = false;
            jsConfig.prettyPrint = false;
            JS::CHTLJSGenerator jsGen(jsConfig);
            content = jsGen.generate(content);
        }
        
        output += content;
        output += "\n\n";
    }
    
    for (const auto& child : node->getChildren()) {
        collectScripts(child, output, config);
    }
    
    // 如果推送了上下文，弹出它
    if (pushedContext) {
        Bridge::SaltBridge::getInstance().popContext();
    }
}

} // namespace CHTL
