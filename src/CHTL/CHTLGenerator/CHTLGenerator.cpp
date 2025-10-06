#include "CHTLGenerator.h"
#include <sstream>
#include <unordered_set>

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
    
    if (config_.prettyPrint) {
        result << html_;
    } else {
        // 简化版：去除多余空白
        result << html_;
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
    if (node.isLocal()) {
        // 局部脚本添加到全局脚本区
        appendJs(node.getContent());
        appendJs("\n");
    } else {
        generateScript(node.getContent());
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

void CHTLGenerator::generateElement(ElementNode& node) {
    if (config_.prettyPrint) {
        appendHtml(indent());
    }
    
    // 开始标签
    appendHtml("<" + node.getTagName());
    
    // 属性
    generateAttributes(node.getAttributes());
    
    // 自闭合标签
    if (isSelfClosingTag(node.getTagName()) && node.getChildren().empty()) {
        appendHtml(" />");
        if (config_.prettyPrint) {
            appendHtml("\n");
        }
        return;
    }
    
    appendHtml(">");
    
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
    String output;
    if (root) {
        generateNode(root, output, 0, config);
    }
    return output;
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
    
    if (node->getType() == NodeType::Script) {
        auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(node);
        output += scriptNode->getContent();
        output += "\n\n";
    }
    
    for (const auto& child : node->getChildren()) {
        collectScripts(child, output, config);
    }
}

} // namespace CHTL
