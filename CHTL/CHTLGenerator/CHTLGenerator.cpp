#include "CHTLGenerator.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : inlineCSS_(false), inlineJS_(false), defaultStructure_(false) {
}

CHTLGenerator::~CHTLGenerator() {
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    html_.clear();
    
    // 处理配置和使用
    processUses(ast);
    processConfigurations(ast);
    
    // 处理导入
    processImports(ast);
    
    // 处理命名空间
    processNamespaces(ast);
    
    // 处理模板和自定义
    processTemplates(ast);
    processCustoms(ast);
    
    // 生成HTML
    html_ = generateElement(ast);
    
    // 优化HTML
    optimizeHTML();
    
    return html_;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    css_.clear();
    
    // 遍历AST收集所有样式
    // TODO: 实现collectStyles方法
    // collectStyles(ast);
    
    // 优化CSS
    optimizeCSS();
    
    return css_;
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    javascript_.clear();
    
    // 遍历AST收集所有脚本
    // TODO: 实现collectScripts方法
    // collectScripts(ast);
    
    // 优化JavaScript
    optimizeJavaScript();
    
    return javascript_;
}

std::string CHTLGenerator::generatePage(std::shared_ptr<BaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    // 生成各部分代码
    std::string html = generateHTML(ast);
    std::string css = generateCSS(ast);
    std::string js = generateJavaScript(ast);
    
    // 生成完整页面
    std::stringstream page;
    
    if (defaultStructure_) {
        page << generateDefaultStructure();
    }
    
    page << generatePageHeader();
    page << html;
    
    if (inlineCSS_) {
        page << "<style>\n" << css << "\n</style>\n";
    }
    
    if (inlineJS_) {
        page << "<script>\n" << js << "\n</script>\n";
    }
    
    page << generatePageFooter();
    
    return page.str();
}

void CHTLGenerator::setInlineCSS(bool inlineCSS) {
    inlineCSS_ = inlineCSS;
}

void CHTLGenerator::setInlineJS(bool inlineJS) {
    inlineJS_ = inlineJS;
}

void CHTLGenerator::setDefaultStructure(bool defaultStructure) {
    defaultStructure_ = defaultStructure;
}

const std::string& CHTLGenerator::getHTML() const {
    return html_;
}

const std::string& CHTLGenerator::getCSS() const {
    return css_;
}

const std::string& CHTLGenerator::getJavaScript() const {
    return javascript_;
}

std::string CHTLGenerator::generateElement(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return "";
    }
    
    std::stringstream html;
    
    switch (node->getType()) {
        case NodeType::ELEMENT: {
            std::string tagName = node->getValue();
            if (tagName.empty()) {
                tagName = "div";
            }
            
            html << "<" << tagName;
            
            // 添加属性
            auto attributes = node->getAttributes();
            if (!attributes.empty()) {
                html << " " << formatAttributes(attributes);
            }
            
            // 处理子节点
            auto children = node->getChildren();
            if (children.empty()) {
                html << " />";
            } else {
                html << ">";
                
                for (const auto& child : children) {
                    html << generateElement(child);
                }
                
                html << "</" << tagName << ">";
            }
            break;
        }
        
        case NodeType::TEXT: {
            html << escapeHTML(node->getValue());
            break;
        }
        
        case NodeType::COMMENT: {
            html << "<!-- " << node->getValue() << " -->";
            break;
        }
        
        case NodeType::STYLE: {
            if (inlineCSS_) {
                html << "<style>\n" << generateStyleProperties(node) << "\n</style>";
            } else {
                processStyleBlock(node);
            }
            break;
        }
        
        case NodeType::SCRIPT: {
            if (inlineJS_) {
                html << "<script>\n" << generateScriptContent(node) << "\n</script>";
            } else {
                processScriptBlock(node);
            }
            break;
        }
        
        case NodeType::TEMPLATE: {
            processTemplate(node);
            break;
        }
        
        case NodeType::CUSTOM: {
            processCustom(node);
            break;
        }
        
        case NodeType::ORIGIN: {
            processOrigin(node);
            break;
        }
        
        case NodeType::IMPORT: {
            processImport(node);
            break;
        }
        
        case NodeType::NAMESPACE: {
            processNamespace(node);
            break;
        }
        
        case NodeType::CONFIG: {
            processConfiguration(node);
            break;
        }
        
        case NodeType::OPERATOR: {
            processUse(node);
            break;
        }
        
        default:
            break;
    }
    
    return html.str();
}

std::string CHTLGenerator::generateTextNode(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::TEXT) {
        return "";
    }
    
    return escapeHTML(node->getValue());
}

std::string CHTLGenerator::generateComment(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::COMMENT) {
        return "";
    }
    
    return "<!-- " + node->getValue() + " -->";
}

std::string CHTLGenerator::generateStyleBlock(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::STYLE) {
        return "";
    }
    
    std::stringstream css;
    css << "<style>\n";
    css << generateStyleProperties(node);
    css << "\n</style>";
    
    return css.str();
}

std::string CHTLGenerator::generateScriptBlock(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::SCRIPT) {
        return "";
    }
    
    std::stringstream js;
    js << "<script>\n";
    js << generateScriptContent(node);
    js << "\n</script>";
    
    return js.str();
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::TEMPLATE) {
        return "";
    }
    
    // 模板在生成时被处理，不直接输出
    return "";
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::CUSTOM) {
        return "";
    }
    
    // 自定义在生成时被处理，不直接输出
    return "";
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::ORIGIN) {
        return "";
    }
    
    // 原始嵌入直接输出内容
    return node->getValue();
}

std::string CHTLGenerator::generateImport(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::IMPORT) {
        return "";
    }
    
    // 导入在生成时被处理，不直接输出
    return "";
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::NAMESPACE) {
        return "";
    }
    
    // 命名空间在生成时被处理，不直接输出
    return "";
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::CONFIG) {
        return "";
    }
    
    // 配置在生成时被处理，不直接输出
    return "";
}

std::string CHTLGenerator::generateUse(std::shared_ptr<BaseNode> node) {
    if (!node || node->getType() != NodeType::OPERATOR) {
        return "";
    }
    
    // 使用在生成时被处理，不直接输出
    return "";
}

std::string CHTLGenerator::generateStyleProperties(std::shared_ptr<BaseNode> styleNode) {
    if (!styleNode || styleNode->getType() != NodeType::STYLE) {
        return "";
    }
    
    std::stringstream css;
    auto properties = styleNode->getAttributes();
    
    for (const auto& prop : properties) {
        css << "  " << prop.first << ": " << prop.second << ";\n";
    }
    
    return css.str();
}

std::string CHTLGenerator::generateStyleSelector(const std::string& selector) {
    return selector;
}

std::string CHTLGenerator::generateStyleRule(const std::string& selector, const std::unordered_map<std::string, std::string>& properties) {
    std::stringstream css;
    css << selector << " {\n";
    
    for (const auto& prop : properties) {
        css << "  " << prop.first << ": " << prop.second << ";\n";
    }
    
    css << "}\n";
    return css.str();
}

std::string CHTLGenerator::generateScriptContent(std::shared_ptr<BaseNode> scriptNode) {
    if (!scriptNode || scriptNode->getType() != NodeType::SCRIPT) {
        return "";
    }
    
    return scriptNode->getValue();
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
    result = std::regex_replace(result, std::regex("&"), "&amp;");
    result = std::regex_replace(result, std::regex("<"), "&lt;");
    result = std::regex_replace(result, std::regex(">"), "&gt;");
    result = std::regex_replace(result, std::regex("\""), "&quot;");
    result = std::regex_replace(result, std::regex("'"), "&#39;");
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& text) {
    // CSS转义相对简单，主要是引号
    std::string result = text;
    result = std::regex_replace(result, std::regex("\""), "\\\"");
    return result;
}

std::string CHTLGenerator::escapeJavaScript(const std::string& text) {
    // JavaScript转义
    std::string result = text;
    result = std::regex_replace(result, std::regex("\\"), "\\\\");
    result = std::regex_replace(result, std::regex("\""), "\\\"");
    result = std::regex_replace(result, std::regex("'"), "\\'");
    result = std::regex_replace(result, std::regex("\n"), "\\n");
    result = std::regex_replace(result, std::regex("\r"), "\\r");
    result = std::regex_replace(result, std::regex("\t"), "\\t");
    
    return result;
}

std::string CHTLGenerator::formatAttributes(const std::unordered_map<std::string, std::string>& attributes) {
    std::stringstream attrs;
    
    for (const auto& attr : attributes) {
        if (!attr.first.empty()) {
            attrs << attr.first;
            if (!attr.second.empty()) {
                attrs << "=\"" << escapeHTML(attr.second) << "\"";
            }
            attrs << " ";
        }
    }
    
    std::string result = attrs.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string CHTLGenerator::formatStyleProperties(const std::unordered_map<std::string, std::string>& properties) {
    std::stringstream props;
    
    for (const auto& prop : properties) {
        if (!prop.first.empty()) {
            props << "  " << prop.first << ": " << prop.second << ";\n";
        }
    }
    
    return props.str();
}

void CHTLGenerator::processTemplate(std::shared_ptr<BaseNode> templateNode) {
    // 模板处理逻辑
    // 这里需要根据模板类型和内容进行相应的处理
}

void CHTLGenerator::processCustom(std::shared_ptr<BaseNode> customNode) {
    // 自定义处理逻辑
    // 这里需要根据自定义类型和内容进行相应的处理
}

void CHTLGenerator::processOrigin(std::shared_ptr<BaseNode> originNode) {
    // 原始嵌入处理逻辑
    // 直接输出原始内容
}

void CHTLGenerator::processImport(std::shared_ptr<BaseNode> importNode) {
    // 导入处理逻辑
    // 这里需要根据导入类型和路径进行相应的处理
}

void CHTLGenerator::processNamespace(std::shared_ptr<BaseNode> namespaceNode) {
    // 命名空间处理逻辑
    // 这里需要根据命名空间内容进行相应的处理
}

void CHTLGenerator::processConfiguration(std::shared_ptr<BaseNode> configNode) {
    // 配置处理逻辑
    // 这里需要根据配置内容进行相应的处理
}

void CHTLGenerator::processUse(std::shared_ptr<BaseNode> useNode) {
    // 使用处理逻辑
    // 这里需要根据使用内容进行相应的处理
}

void CHTLGenerator::processStyleBlock(std::shared_ptr<BaseNode> styleNode) {
    // 样式块处理逻辑
    // 这里需要根据样式块内容进行相应的处理
}

void CHTLGenerator::processLocalStyle(std::shared_ptr<BaseNode> styleNode, std::shared_ptr<BaseNode> parentElement) {
    // 局部样式处理逻辑
    // 这里需要根据局部样式内容进行相应的处理
}

void CHTLGenerator::processGlobalStyle(std::shared_ptr<BaseNode> styleNode) {
    // 全局样式处理逻辑
    // 这里需要根据全局样式内容进行相应的处理
}

void CHTLGenerator::processScriptBlock(std::shared_ptr<BaseNode> scriptNode) {
    // 脚本块处理逻辑
    // 这里需要根据脚本块内容进行相应的处理
}

void CHTLGenerator::processLocalScript(std::shared_ptr<BaseNode> scriptNode, std::shared_ptr<BaseNode> parentElement) {
    // 局部脚本处理逻辑
    // 这里需要根据局部脚本内容进行相应的处理
}

void CHTLGenerator::processGlobalScript(std::shared_ptr<BaseNode> scriptNode) {
    // 全局脚本处理逻辑
    // 这里需要根据全局脚本内容进行相应的处理
}

void CHTLGenerator::processTemplates(std::shared_ptr<BaseNode> ast) {
    // 模板处理逻辑
    // 这里需要遍历AST并处理所有模板
}

void CHTLGenerator::processCustoms(std::shared_ptr<BaseNode> ast) {
    // 自定义处理逻辑
    // 这里需要遍历AST并处理所有自定义
}

void CHTLGenerator::processImports(std::shared_ptr<BaseNode> ast) {
    // 导入处理逻辑
    // 这里需要遍历AST并处理所有导入
}

void CHTLGenerator::processNamespaces(std::shared_ptr<BaseNode> ast) {
    // 命名空间处理逻辑
    // 这里需要遍历AST并处理所有命名空间
}

void CHTLGenerator::processConfigurations(std::shared_ptr<BaseNode> ast) {
    // 配置处理逻辑
    // 这里需要遍历AST并处理所有配置
}

void CHTLGenerator::processUses(std::shared_ptr<BaseNode> ast) {
    // 使用处理逻辑
    // 这里需要遍历AST并处理所有使用
}

std::string CHTLGenerator::generateDefaultStructure() {
    return "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>CHTL Page</title>\n</head>\n<body>\n";
}

std::string CHTLGenerator::generatePageHeader() {
    return "";
}

std::string CHTLGenerator::generatePageFooter() {
    return "</body>\n</html>";
}

void CHTLGenerator::optimizeHTML() {
    // HTML优化逻辑
    // 这里可以添加HTML优化功能
}

void CHTLGenerator::optimizeCSS() {
    // CSS优化逻辑
    // 这里可以添加CSS优化功能
}

void CHTLGenerator::optimizeJavaScript() {
    // JavaScript优化逻辑
    // 这里可以添加JavaScript优化功能
}

bool CHTLGenerator::validateHTML(const std::string& html) {
    // HTML验证逻辑
    // 这里可以添加HTML验证功能
    return true;
}

bool CHTLGenerator::validateCSS(const std::string& css) {
    // CSS验证逻辑
    // 这里可以添加CSS验证功能
    return true;
}

bool CHTLGenerator::validateJavaScript(const std::string& javascript) {
    // JavaScript验证逻辑
    // 这里可以添加JavaScript验证功能
    return true;
}

} // namespace CHTL