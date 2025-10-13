#pragma once

#include "../Common.hpp"
#include "Node.hpp"

namespace CHTL {

// HTML生成器
class HTMLGenerator : public NodeVisitor {
public:
    HTMLGenerator() : indent_(0), inlineCSS_(false), inlineJS_(false) {}
    
    String generate(Ptr<Node> root) {
        html_.clear();
        css_.clear();
        js_.clear();
        indent_ = 0;
        
        root->accept(*this);
        
        return assembleOutput();
    }
    
    void setInlineCSS(bool inline) { inlineCSS_ = inline; }
    void setInlineJS(bool inline) { inlineJS_ = inline; }
    
    const String& getHTML() const { return html_; }
    const String& getCSS() const { return css_; }
    const String& getJS() const { return js_; }
    
    // 访问者方法实现
    void visitElement(ElementNode& node) override {
        // 跳过root元素
        if (node.tagName() == "root") {
            for (auto& child : node.children()) {
                child->accept(*this);
            }
            return;
        }
        
        // 生成开始标签
        writeIndent();
        html_ += "<" + node.tagName();
        
        // 添加属性
        for (const auto& [name, value] : node.attributes()) {
            html_ += " " + name + "=\"" + escapeHTML(value) + "\"";
        }
        
        // 收集内联样式
        String inlineStyle;
        Vec<Ptr<Node>> otherChildren;
        
        for (auto& child : node.children()) {
            if (child->type() == NodeType::Style) {
                auto styleNode = std::static_pointer_cast<StyleNode>(child);
                for (const auto& [prop, value] : styleNode->properties()) {
                    if (!inlineStyle.empty()) inlineStyle += "; ";
                    inlineStyle += prop + ": " + value;
                }
            } else {
                otherChildren.push_back(child);
            }
        }
        
        if (!inlineStyle.empty()) {
            html_ += " style=\"" + inlineStyle + "\"";
        }
        
        // 自闭合标签
        if (isSelfClosingTag(node.tagName()) && otherChildren.empty()) {
            html_ += " />\n";
            return;
        }
        
        html_ += ">";
        
        // 处理子节点
        if (!otherChildren.empty()) {
            bool hasBlockChild = false;
            for (const auto& child : otherChildren) {
                if (child->type() == NodeType::Element) {
                    hasBlockChild = true;
                    break;
                }
            }
            
            if (hasBlockChild) {
                html_ += "\n";
                indent_++;
            }
            
            for (auto& child : otherChildren) {
                child->accept(*this);
            }
            
            if (hasBlockChild) {
                indent_--;
                writeIndent();
            }
        }
        
        // 生成结束标签
        html_ += "</" + node.tagName() + ">\n";
    }
    
    void visitText(TextNode& node) override {
        html_ += escapeHTML(node.content());
    }
    
    void visitComment(CommentNode& node) override {
        if (node.isGenerator()) {
            // 生成器注释 - 根据上下文生成不同类型的注释
            writeIndent();
            html_ += "<!-- " + escapeHTML(node.content()) + " -->\n";
        }
        // 普通注释不输出到HTML
    }
    
    void visitStyle(StyleNode& node) override {
        // 局部样式已在visitElement中处理
        // 这里处理全局样式
        for (const auto& [prop, value] : node.properties()) {
            css_ += "  " + prop + ": " + value + ";\n";
        }
    }
    
    void visitScript(ScriptNode& node) override {
        js_ += node.content() + "\n";
    }
    
    void visitTemplate(TemplateNode& node) override {
        // 模板在代码生成阶段已经被展开，这里不需要处理
        for (auto& child : node.children()) {
            child->accept(*this);
        }
    }
    
    void visitCustom(CustomNode& node) override {
        // 自定义元素在代码生成阶段已经被展开
        for (auto& child : node.children()) {
            child->accept(*this);
        }
    }
    
    void visitOrigin(OriginNode& node) override {
        // 原始嵌入直接输出内容
        const String& type = node.originType();
        
        if (type == "@Html" || type == "@html") {
            writeIndent();
            html_ += node.content() + "\n";
        } else if (type == "@Style" || type == "@CSS") {
            css_ += node.content() + "\n";
        } else if (type == "@JavaScript" || type == "@JS") {
            js_ += node.content() + "\n";
        } else {
            // 自定义类型的原始嵌入
            writeIndent();
            html_ += "<!-- Custom Origin: " + type + " -->\n";
            writeIndent();
            html_ += node.content() + "\n";
        }
    }
    
    void visitImport(ImportNode& node) override {
        // 导入在编译时处理，这里生成引用
        const String& from = node.from();
        const String& type = node.importType();
        
        if (type.find("@Style") != String::npos || type.find("@CSS") != String::npos) {
            writeIndent();
            html_ += "<link rel=\"stylesheet\" href=\"" + from + "\">\n";
        } else if (type.find("@JavaScript") != String::npos || type.find("@JS") != String::npos) {
            writeIndent();
            html_ += "<script src=\"" + from + "\"></script>\n";
        }
    }
    
    void visitNamespace(NamespaceNode& node) override {
        // 命名空间在编译时处理
        for (auto& child : node.children()) {
            child->accept(*this);
        }
    }
    
    void visitConfiguration(ConfigurationNode& node) override {
        // 配置在编译时处理，不输出到HTML
    }
    
    void visitExpression(ExpressionNode& node) override {
        html_ += node.expression();
    }
    
    void visitAttribute(AttributeNode& node) override {
        // 属性在visitElement中处理
    }
    
private:
    String html_;
    String css_;
    String js_;
    int indent_;
    bool inlineCSS_;
    bool inlineJS_;
    
    void writeIndent() {
        for (int i = 0; i < indent_; ++i) {
            html_ += "  ";
        }
    }
    
    String escapeHTML(const String& text) const {
        String result;
        for (char c : text) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                default: result += c;
            }
        }
        return result;
    }
    
    bool isSelfClosingTag(const String& tag) const {
        static const Vec<String> selfClosing = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        };
        
        return std::find(selfClosing.begin(), selfClosing.end(), tag) != selfClosing.end();
    }
    
    String assembleOutput() {
        String output = html_;
        
        // 添加CSS
        if (!css_.empty()) {
            if (inlineCSS_) {
                output = "<style>\n" + css_ + "</style>\n" + output;
            }
        }
        
        // 添加JS
        if (!js_.empty()) {
            if (inlineJS_) {
                output += "<script>\n" + js_ + "</script>\n";
            }
        }
        
        return output;
    }
};

// 代码生成器上下文
class GeneratorContext {
public:
    GeneratorContext() = default;
    
    void addTemplate(const String& name, Ptr<Node> node) {
        templates_[name] = node;
    }
    
    Opt<Ptr<Node>> getTemplate(const String& name) const {
        auto it = templates_.find(name);
        if (it != templates_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    void addCustom(const String& name, Ptr<Node> node) {
        customs_[name] = node;
    }
    
    Opt<Ptr<Node>> getCustom(const String& name) const {
        auto it = customs_.find(name);
        if (it != customs_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    void setConfiguration(const String& key, const String& value) {
        config_[key] = value;
    }
    
    Opt<String> getConfiguration(const String& key) const {
        auto it = config_.find(key);
        if (it != config_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
private:
    Map<String, Ptr<Node>> templates_;
    Map<String, Ptr<Node>> customs_;
    Map<String, String> config_;
};

} // namespace CHTL
