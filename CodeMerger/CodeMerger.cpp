#include "CodeMerger.h"
#include <fstream>
#include <iostream>
#include <filesystem>

CodeMerger::CodeMerger() {
}

bool CodeMerger::merge(const CompiledCode& compiled, std::string& finalOutput, 
                      bool inlineMode, bool inlineCSS, bool inlineJS, bool defaultStruct) {
    std::cout << "开始合并代码..." << std::endl;
    
    try {
        // 生成HTML结构
        finalOutput = generateHTMLStructure(compiled, inlineMode, inlineCSS, inlineJS, defaultStruct);
        
        // 如果不是内联模式，生成独立的CSS和JS文件
        if (!inlineMode) {
            if (!inlineCSS && !compiled.cssCode.empty()) {
                // 生成独立的CSS文件
                std::string baseName = "output"; // 这里应该从输入文件名获取
                generateCSSFile(compiled.cssCode, baseName);
            }
            
            if (!inlineJS && !compiled.jsCode.empty()) {
                // 生成独立的JS文件
                std::string baseName = "output";
                generateJSFile(compiled.jsCode, baseName);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "代码合并失败: " << e.what() << std::endl;
        return false;
    }
}

std::string CodeMerger::generateHTMLStructure(const CompiledCode& compiled, 
                                             bool inlineMode, bool inlineCSS, bool inlineJS, 
                                             bool defaultStruct) {
    std::string html;
    
    if (defaultStruct) {
        html += generateDefaultHTMLStructure();
    } else {
        // 使用编译后的CHTL代码作为基础
        html = compiled.chtlCode;
    }
    
    // 添加CSS
    if (!compiled.cssCode.empty()) {
        if (inlineCSS || inlineMode) {
            std::string cssTag = this->inlineCSS(compiled.cssCode);
            size_t headPos = html.find("</head>");
            if (headPos != std::string::npos) {
                html.insert(headPos, cssTag);
            } else {
                html = cssTag + html;
            }
        } else {
            // 添加CSS链接
            std::string cssLink = "<link rel=\"stylesheet\" href=\"output.css\">\n";
            size_t headPos = html.find("</head>");
            if (headPos != std::string::npos) {
                html.insert(headPos, cssLink);
            } else {
                html = cssLink + html;
            }
        }
    }
    
    // 添加JavaScript
    if (!compiled.jsCode.empty()) {
        if (inlineJS || inlineMode) {
            std::string jsTag = this->inlineJS(compiled.jsCode);
            size_t bodyEndPos = html.find("</body>");
            if (bodyEndPos != std::string::npos) {
                html.insert(bodyEndPos, jsTag);
            } else {
                html += jsTag;
            }
        } else {
            // 添加JS链接
            std::string jsLink = "<script src=\"output.js\"></script>\n";
            size_t bodyEndPos = html.find("</body>");
            if (bodyEndPos != std::string::npos) {
                html.insert(bodyEndPos, jsLink);
            } else {
                html += jsLink;
            }
        }
    }
    
    return html;
}

std::string CodeMerger::generateCSS(const CompiledCode& compiled, bool inlineMode) {
    return compiled.cssCode;
}

std::string CodeMerger::generateJS(const CompiledCode& compiled, bool inlineMode) {
    return compiled.jsCode;
}

std::string CodeMerger::generateDefaultHTMLStructure() {
    return R"(<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Generated Page</title>
</head>
<body>
    <!-- CHTL生成的内容将在这里 -->
</body>
</html>)";
}

std::string CodeMerger::inlineCSS(const std::string& css) {
    // 返回内联的CSS标签
    return "<style>\n" + css + "\n</style>\n";
}

std::string CodeMerger::inlineJS(const std::string& js) {
    // 返回内联的JavaScript标签
    return "<script>\n" + js + "\n</script>\n";
}

bool CodeMerger::generateCSSFile(const std::string& css, const std::string& baseName) {
    std::string filename = baseName + ".css";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建CSS文件: " << filename << std::endl;
        return false;
    }
    
    file << css;
    file.close();
    
    std::cout << "CSS文件已生成: " << filename << std::endl;
    return true;
}

bool CodeMerger::generateJSFile(const std::string& js, const std::string& baseName) {
    std::string filename = baseName + ".js";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建JS文件: " << filename << std::endl;
        return false;
    }
    
    file << js;
    file.close();
    
    std::cout << "JavaScript文件已生成: " << filename << std::endl;
    return true;
}