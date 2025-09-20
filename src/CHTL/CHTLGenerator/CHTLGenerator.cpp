#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : indentSize(2), useSpaces(true), minifyOutput(false), 
      includeComments(true), defaultHTMLStructure(true), debugMode(false), currentIndent(0) {
    
    // 设置默认模板
    htmlTemplate = GeneratorUtils::getDefaultHTMLTemplate();
    cssTemplate = GeneratorUtils::getDefaultCSSTemplate();
    jsTemplate = GeneratorUtils::getDefaultJSTemplate();
}

CHTLGenerator::~CHTLGenerator() {
}

GenerationResult CHTLGenerator::generate(const std::shared_ptr<ASTNode>& ast) {
    GenerationResult result;
    errors.clear();
    currentIndent = 0;
    generatedIDs.clear();
    globalStyles.clear();
    globalScripts.clear();
    
    debugPrint("Starting code generation");
    
    try {
        if (!ast) {
            error("AST is null");
            return result;
        }
        
        // 生成HTML
        result.html = generateHTML(ast);
        if (hasErrors()) {
            return result;
        }
        
        // 生成CSS
        result.css = generateCSS(ast);
        if (hasErrors()) {
            return result;
        }
        
        // 生成JS
        result.js = generateJS(ast);
        if (hasErrors()) {
            return result;
        }
        
        result.success = true;
        debugPrint("Code generation completed successfully");
        
    } catch (const std::exception& e) {
        error("Generation error: " + std::string(e.what()));
    }
    
    return result;
}

std::string CHTLGenerator::generateHTML(const std::shared_ptr<ASTNode>& ast) {
    debugPrint("Generating HTML");
    
    std::stringstream html;
    
    if (defaultHTMLStructure && ast->getType() == NodeType::DOCUMENT) {
        html << "<!DOCTYPE html>\n";
    }
    
    currentIndent = 0;
    html << generateElement(ast);
    
    std::string result = html.str();
    
    // 应用HTML模板
    if (!htmlTemplate.empty()) {
        result = std::regex_replace(htmlTemplate, std::regex("\\{\\{CONTENT\\}\\}"), result);
    }
    
    // 格式化或压缩
    if (minifyOutput) {
        result = GeneratorUtils::minifyHTML(result);
    } else if (!minifyOutput && includeComments) {
        result = GeneratorUtils::formatHTML(result, indentSize);
    }
    
    debugPrint("HTML generation completed");
    return result;
}

std::string CHTLGenerator::generateCSS(const std::shared_ptr<ASTNode>& ast) {
    debugPrint("Generating CSS");
    
    std::stringstream css;
    
    currentIndent = 0;
    
    // 生成全局样式
    for (const auto& style : globalStyles) {
        css << style << "\n";
    }
    
    // 生成局部样式
    generateCSSRecursive(ast, css);
    
    std::string result = css.str();
    
    // 应用CSS模板
    if (!cssTemplate.empty()) {
        result = std::regex_replace(cssTemplate, std::regex("\\{\\{CONTENT\\}\\}"), result);
    }
    
    // 格式化或压缩
    if (minifyOutput) {
        result = GeneratorUtils::minifyCSS(result);
    } else {
        result = GeneratorUtils::formatCSS(result, indentSize);
    }
    
    debugPrint("CSS generation completed");
    return result;
}

std::string CHTLGenerator::generateJS(const std::shared_ptr<ASTNode>& ast) {
    debugPrint("Generating JavaScript");
    
    std::stringstream js;
    
    currentIndent = 0;
    
    // 生成全局脚本
    for (const auto& script : globalScripts) {
        js << script << "\n";
    }
    
    // 生成局部脚本
    generateJSRecursive(ast, js);
    
    std::string result = js.str();
    
    // 应用JS模板
    if (!jsTemplate.empty()) {
        result = std::regex_replace(jsTemplate, std::regex("\\{\\{CONTENT\\}\\}"), result);
    }
    
    // 格式化或压缩
    if (minifyOutput) {
        result = GeneratorUtils::minifyJS(result);
    } else {
        result = GeneratorUtils::formatJS(result, indentSize);
    }
    
    debugPrint("JavaScript generation completed");
    return result;
}

std::string CHTLGenerator::generateElement(const std::shared_ptr<ASTNode>& node) {
    if (!node) return "";
    
    std::stringstream result;
    std::string indent = getIndent(currentIndent);
    
    switch (node->getType()) {
        case NodeType::DOCUMENT: {
            // 处理文档节点
            for (size_t i = 0; i < node->getChildCount(); ++i) {
                auto child = node->getChild(i);
                result << generateElement(child);
                if (i < node->getChildCount() - 1) {
                    result << "\n";
                }
            }
            break;
        }
        
        case NodeType::ELEMENT: {
            std::string tagName = node->getValue();
            
            // 生成开始标签
            result << indent << "<" << tagName;
            
            // 生成属性
            std::string attributes = generateAttributes(node);
            if (!attributes.empty()) {
                result << " " << attributes;
            }
            
            // 检查是否有子节点
            bool hasChildren = false;
            std::string content;
            
            for (size_t i = 0; i < node->getChildCount(); ++i) {
                auto child = node->getChild(i);
                if (child->getType() == NodeType::ATTRIBUTE) {
                    // 属性已经处理过了
                    continue;
                }
                
                hasChildren = true;
                if (child->getType() == NodeType::TEXT) {
                    content += generateTextNode(child);
                } else if (child->getType() == NodeType::LOCAL_STYLE) {
                    // 局部样式将在CSS中生成
                    continue;
                } else if (child->getType() == NodeType::SCRIPT) {
                    // 脚本将在JS中生成
                    continue;
                } else {
                    content += generateElement(child);
                }
            }
            
            // 生成结束标签
            if (hasChildren) {
                if (content.find('\n') != std::string::npos) {
                    // 多行内容
                    result << ">\n" << content;
                    if (!minifyOutput) {
                        result << indent;
                    }
                    result << "</" << tagName << ">";
                } else {
                    // 单行内容
                    result << ">" << content << "</" << tagName << ">";
                }
            } else {
                // 自闭合标签或空标签
                if (isSelfClosingTag(tagName)) {
                    result << " />";
                } else {
                    result << "></" << tagName << ">";
                }
            }
            break;
        }
        
        case NodeType::TEXT: {
            result << generateTextNode(node);
            break;
        }
        
        case NodeType::COMMENT: {
            if (includeComments) {
                result << generateComment(node);
            }
            break;
        }
        
        case NodeType::GENERATOR_COMMENT: {
            if (includeComments) {
                result << generateGeneratorComment(node);
            }
            break;
        }
        
        default:
            // 其他节点类型将在后续实现
            break;
    }
    
    return result.str();
}

std::string CHTLGenerator::generateAttributes(const std::shared_ptr<ASTNode>& node) {
    std::stringstream attributes;
    bool first = true;
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child->getType() == NodeType::ATTRIBUTE) {
            if (!first) {
                attributes << " ";
            }
            attributes << generateAttribute(child);
            first = false;
        }
    }
    
    return attributes.str();
}

std::string CHTLGenerator::generateAttribute(const std::shared_ptr<ASTNode>& node) {
    std::string name = node->getValue();
    std::string value = node->getContent();
    
    if (value.empty()) {
        return name; // 布尔属性
    }
    
    return name + "=\"" + escapeHTML(value) + "\"";
}

std::string CHTLGenerator::generateTextNode(const std::shared_ptr<ASTNode>& node) {
    std::string content = node->getValue();
    return escapeHTML(content);
}

std::string CHTLGenerator::generateComment(const std::shared_ptr<ASTNode>& node) {
    std::string content = node->getValue();
    std::string indent = getIndent(currentIndent);
    return indent + "<!-- " + content + " -->";
}

std::string CHTLGenerator::generateGeneratorComment(const std::shared_ptr<ASTNode>& node) {
    std::string content = node->getValue();
    std::string indent = getIndent(currentIndent);
    
    // 根据上下文生成不同类型的注释
    // 这里简化处理，都生成HTML注释
    return indent + "<!-- " + content + " -->";
}

std::string CHTLGenerator::generateLocalStyle(const std::shared_ptr<ASTNode>& node) {
    std::stringstream css;
    
    // 生成CSS选择器（这里简化处理）
    std::string selector = generateCSSSelector(nullptr);
    
    css << selector << " {\n";
    currentIndent++;
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child->getType() == NodeType::STYLE_PROPERTY) {
            css << generateStyleProperty(child);
            if (!minifyOutput) {
                css << "\n";
            }
        }
    }
    
    currentIndent--;
    std::string indent = getIndent(currentIndent);
    css << indent << "}";
    
    return css.str();
}

std::string CHTLGenerator::generateStyleProperty(const std::shared_ptr<ASTNode>& node) {
    std::string property = node->getValue();
    std::string value = node->getContent();
    std::string indent = getIndent(currentIndent);
    
    return indent + property + ": " + escapeCSS(value) + ";";
}

std::string CHTLGenerator::generateScript(const std::shared_ptr<ASTNode>& node) {
    std::string content = node->getValue();
    
    // 检查是否包含CHTL JS语法
    if (content.find("{{") != std::string::npos) {
        return generateCHTLJSCode(content);
    }
    
    return content;
}

std::string CHTLGenerator::generateCHTLJSCode(const std::string& source) {
    // CHTL JS代码生成将在后续实现
    // 这里先返回原始代码
    return source;
}

void CHTLGenerator::generateCSSRecursive(const std::shared_ptr<ASTNode>& node, std::stringstream& css) {
    if (!node) return;
    
    if (node->getType() == NodeType::LOCAL_STYLE) {
        css << generateLocalStyle(node);
        if (!minifyOutput) {
            css << "\n\n";
        }
    }
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        generateCSSRecursive(node->getChild(i), css);
    }
}

void CHTLGenerator::generateJSRecursive(const std::shared_ptr<ASTNode>& node, std::stringstream& js) {
    if (!node) return;
    
    if (node->getType() == NodeType::SCRIPT) {
        js << generateScript(node);
        if (!minifyOutput) {
            js << "\n\n";
        }
    }
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        generateJSRecursive(node->getChild(i), js);
    }
}

// 辅助方法实现
std::string CHTLGenerator::getIndent(size_t level) {
    if (minifyOutput) return "";
    
    std::string indent;
    size_t spaces = level * indentSize;
    
    if (useSpaces) {
        indent.assign(spaces, ' ');
    } else {
        indent.assign(level, '\t');
    }
    
    return indent;
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 转义HTML特殊字符
    result = std::regex_replace(result, std::regex("&"), "&amp;");
    result = std::regex_replace(result, std::regex("<"), "&lt;");
    result = std::regex_replace(result, std::regex(">"), "&gt;");
    result = std::regex_replace(result, std::regex("\""), "&quot;");
    result = std::regex_replace(result, std::regex("'"), "&#39;");
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& text) {
    std::string result = text;
    
    // CSS转义将在后续实现
    return result;
}

std::string CHTLGenerator::escapeJS(const std::string& text) {
    std::string result = text;
    
    // JavaScript转义将在后续实现
    return result;
}

std::string CHTLGenerator::generateCSSSelector(const std::shared_ptr<ASTNode>& element) {
    if (!element) return "";
    
    std::string selector;
    
    // 生成ID选择器
    for (size_t i = 0; i < element->getChildCount(); ++i) {
        auto child = element->getChild(i);
        if (child->getType() == NodeType::ATTRIBUTE && child->getValue() == "id") {
            selector = "#" + child->getContent();
            break;
        }
    }
    
    // 如果没有ID，生成类选择器
    if (selector.empty()) {
        for (size_t i = 0; i < element->getChildCount(); ++i) {
            auto child = element->getChild(i);
            if (child->getType() == NodeType::ATTRIBUTE && child->getValue() == "class") {
                selector = "." + child->getContent();
                break;
            }
        }
    }
    
    // 如果都没有，生成元素选择器
    if (selector.empty()) {
        selector = element->getValue();
    }
    
    return selector;
}

std::string CHTLGenerator::generateUniqueID(const std::string& prefix) {
    static size_t counter = 0;
    std::string id = prefix + "_" + std::to_string(++counter);
    generatedIDs[id] = id;
    return id;
}

bool CHTLGenerator::isSelfClosingTag(const std::string& tagName) {
    static const std::vector<std::string> selfClosingTags = {
        "img", "br", "hr", "input", "meta", "link", "area", "base",
        "col", "embed", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) != selfClosingTags.end();
}

void CHTLGenerator::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler = handler;
}

void CHTLGenerator::error(const std::string& message) {
    errors.push_back(message);
    if (errorHandler) {
        errorHandler(message);
    }
    debugPrint("ERROR: " + message);
}

void CHTLGenerator::debugPrint(const std::string& message) {
    std::cout << "[CHTLGenerator] " << message << std::endl;
}

// GeneratorUtils 实现
namespace GeneratorUtils {

bool validateHTML(const std::string& html) {
    // HTML验证将在后续实现
    return !html.empty();
}

bool validateCSS(const std::string& css) {
    // CSS验证将在后续实现
    return !css.empty();
}

bool validateJS(const std::string& js) {
    // JavaScript验证将在后续实现
    return !js.empty();
}

std::string formatHTML(const std::string& html, size_t indentSize) {
    // HTML格式化将在后续实现
    return html;
}

std::string formatCSS(const std::string& css, size_t indentSize) {
    // CSS格式化将在后续实现
    return css;
}

std::string formatJS(const std::string& js, size_t indentSize) {
    // JavaScript格式化将在后续实现
    return js;
}

std::string minifyHTML(const std::string& html) {
    std::string result = html;
    
    // 基本的HTML压缩
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex(">\\s+<"), "><");
    
    return result;
}

std::string minifyCSS(const std::string& css) {
    std::string result = css;
    
    // 基本的CSS压缩
    result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex(";\\s*}"), "}");
    
    return result;
}

std::string minifyJS(const std::string& js) {
    std::string result = js;
    
    // 基本的JavaScript压缩
    result = std::regex_replace(result, std::regex("//.*"), "");
    result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    
    return result;
}

std::string inlineCSSIntoHTML(const std::string& html, const std::string& css) {
    std::string result = html;
    
    // 在</head>之前插入CSS
    size_t pos = result.find("</head>");
    if (pos != std::string::npos) {
        std::string styleTag = "<style>\n" + css + "\n</style>\n";
        result.insert(pos, styleTag);
    }
    
    return result;
}

std::string inlineJSIntoHTML(const std::string& html, const std::string& js) {
    std::string result = html;
    
    // 在</body>之前插入JavaScript
    size_t pos = result.find("</body>");
    if (pos != std::string::npos) {
        std::string scriptTag = "<script>\n" + js + "\n</script>\n";
        result.insert(pos, scriptTag);
    }
    
    return result;
}

std::string getDefaultHTMLTemplate() {
    return R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Document</title>
</head>
<body>
{{CONTENT}}
</body>
</html>)";
}

std::string getDefaultCSSTemplate() {
    return R"(/* CHTL Generated CSS */
{{CONTENT}})";
}

std::string getDefaultJSTemplate() {
    return R"(// CHTL Generated JavaScript
{{CONTENT}})";
}

} // namespace GeneratorUtils

} // namespace CHTL