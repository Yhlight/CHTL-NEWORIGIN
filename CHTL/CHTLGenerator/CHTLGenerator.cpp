#include "CHTLGenerator.h"
#include <iostream>
#include <sstream>

CHTLGenerator::CHTLGenerator() 
    : debugMode_(false)
    , indentSize_(2)
    , minify_(false)
    , currentIndent_(0) {
}

bool CHTLGenerator::generate(const std::unique_ptr<CHTLNode>& ast, std::string& output) {
    if (!ast) {
        reportError("AST为空");
        return false;
    }
    
    output_.clear();
    currentIndent_ = 0;
    
    if (debugMode_) {
        std::cout << "开始生成HTML代码..." << std::endl;
    }
    
    generateNode(*ast);
    output = output_;
    
    if (debugMode_) {
        std::cout << "HTML代码生成完成，长度: " << output.length() << std::endl;
    }
    
    return true;
}

void CHTLGenerator::generateNode(const CHTLNode& node) {
    switch (node.getType()) {
        case NodeType::ROOT:
            // 根节点，生成所有子节点
            for (const auto& child : node.getChildren()) {
                generateNode(*child);
            }
            break;
            
        case NodeType::ELEMENT:
            generateElement(static_cast<const ElementNode&>(node));
            break;
            
        case NodeType::TEXT:
            generateText(static_cast<const TextNode&>(node));
            break;
            
        case NodeType::TEMPLATE:
            generateTemplate(static_cast<const TemplateNode&>(node));
            break;
            
        case NodeType::CUSTOM:
            generateCustom(static_cast<const CustomNode&>(node));
            break;
            
        case NodeType::STYLE:
            generateStyle(static_cast<const StyleNode&>(node));
            break;
            
        case NodeType::SCRIPT:
            generateScript(static_cast<const ScriptNode&>(node));
            break;
            
        case NodeType::EXPRESSION:
            generateExpression(static_cast<const ExpressionNode&>(node));
            break;
            
        case NodeType::CONDITIONAL:
            generateConditional(static_cast<const ConditionalNode&>(node));
            break;
            
        case NodeType::OPERATION:
            generateOperation(static_cast<const OperationNode&>(node));
            break;
            
        default:
            if (debugMode_) {
                std::cout << "未知节点类型: " << static_cast<int>(node.getType()) << std::endl;
            }
            break;
    }
}

void CHTLGenerator::generateElement(const ElementNode& node) {
    addIndent();
    addText("<" + node.getTagName());
    
    // 生成属性
    for (const auto& attr : node.getAttributes()) {
        addAttribute(attr.first, attr.second);
    }
    
    if (node.getChildren().empty()) {
        // 自闭合标签
        addText(" />");
        addLine();
    } else {
        addText(">");
        addLine();
        
        // 生成子节点
        currentIndent_++;
        for (const auto& child : node.getChildren()) {
            generateNode(*child);
        }
        currentIndent_--;
        
        addIndent();
        addText("</" + node.getTagName() + ">");
        addLine();
    }
}

void CHTLGenerator::generateText(const TextNode& node) {
    addIndent();
    addText(escapeHtml(node.getValue()));
    addLine();
}

void CHTLGenerator::generateTemplate(const TemplateNode& node) {
    // 模板节点在生成时会被展开
    for (const auto& child : node.getChildren()) {
        generateNode(*child);
    }
}

void CHTLGenerator::generateCustom(const CustomNode& node) {
    // 自定义节点在生成时会被展开
    for (const auto& child : node.getChildren()) {
        generateNode(*child);
    }
}

void CHTLGenerator::generateStyle(const StyleNode& node) {
    addIndent();
    addText("<style>");
    addLine();
    
    // 生成样式内容
    currentIndent_++;
    for (const auto& child : node.getChildren()) {
        generateNode(*child);
    }
    currentIndent_--;
    
    addIndent();
    addText("</style>");
    addLine();
}

void CHTLGenerator::generateScript(const ScriptNode& node) {
    addIndent();
    addText("<script>");
    addLine();
    
    // 生成脚本内容
    currentIndent_++;
    for (const auto& child : node.getChildren()) {
        generateNode(*child);
    }
    currentIndent_--;
    
    addIndent();
    addText("</script>");
    addLine();
}

void CHTLGenerator::generateExpression(const ExpressionNode& node) {
    // 表达式节点在生成时会被计算
    addText(node.getValue());
}

void CHTLGenerator::generateConditional(const ConditionalNode& node) {
    // 条件表达式节点在生成时会被计算
    if (node.getChildren().size() >= 3) {
        generateNode(*node.getChildren()[0]); // 条件
        addText(" ? ");
        generateNode(*node.getChildren()[1]); // 真值
        addText(" : ");
        generateNode(*node.getChildren()[2]); // 假值
    }
}

void CHTLGenerator::generateOperation(const OperationNode& node) {
    // 运算表达式节点在生成时会被计算
    if (node.getChildren().size() >= 2) {
        generateNode(*node.getChildren()[0]); // 左操作数
        addText(" " + node.getOperator() + " ");
        generateNode(*node.getChildren()[1]); // 右操作数
    }
}

void CHTLGenerator::addIndent() {
    if (!minify_) {
        for (int i = 0; i < currentIndent_ * indentSize_; i++) {
            output_ += " ";
        }
    }
}

void CHTLGenerator::addLine() {
    if (!minify_) {
        output_ += "\n";
    }
}

void CHTLGenerator::addText(const std::string& text) {
    output_ += text;
}

void CHTLGenerator::addAttribute(const std::string& name, const std::string& value) {
    addText(" " + name + "=\"" + escapeAttribute(value) + "\"");
}

std::string CHTLGenerator::escapeHtml(const std::string& text) {
    std::string result = text;
    
    // 转义HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string CHTLGenerator::escapeAttribute(const std::string& text) {
    std::string result = text;
    
    // 转义属性值中的特殊字符
    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

void CHTLGenerator::reportError(const std::string& message) {
    std::cerr << "生成错误: " << message << std::endl;
    errors_.push_back(message);
}