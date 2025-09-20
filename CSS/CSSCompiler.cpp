#include "CSSCompiler.h"
#include <iostream>
#include <sstream>
#include <regex>

CSSCompiler::CSSCompiler() 
    : minify_(false)
    , debugMode_(false) {
}

bool CSSCompiler::compile(const std::string& input, std::string& output) {
    std::cout << "开始编译CSS代码..." << std::endl;
    
    try {
        if (minify_) {
            return minifyCSS(input, output);
        } else {
            return parseCSS(input, output);
        }
    } catch (const std::exception& e) {
        std::cerr << "CSS编译过程中发生错误: " << e.what() << std::endl;
        return false;
    }
}

bool CSSCompiler::parseCSS(const std::string& input, std::string& output) {
    // 简单的CSS解析和验证
    // 这里可以使用ANTLR4或其他CSS解析库
    
    if (debugMode_) {
        std::cout << "解析CSS代码，长度: " << input.length() << std::endl;
    }
    
    // 基本的CSS语法检查
    std::regex cssRule(R"([^{}]+{[^{}]*})");
    std::sregex_iterator begin(input.begin(), input.end(), cssRule);
    std::sregex_iterator end;
    
    size_t ruleCount = 0;
    for (auto it = begin; it != end; ++it) {
        ruleCount++;
        if (debugMode_) {
            std::cout << "找到CSS规则 " << ruleCount << ": " << it->str() << std::endl;
        }
    }
    
    if (debugMode_) {
        std::cout << "共找到 " << ruleCount << " 个CSS规则" << std::endl;
    }
    
    // 直接输出，这里可以添加更复杂的处理
    output = input;
    return true;
}

bool CSSCompiler::minifyCSS(const std::string& input, std::string& output) {
    // 简单的CSS压缩
    std::string minified = input;
    
    // 移除注释
    std::regex commentRegex(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)");
    minified = std::regex_replace(minified, commentRegex, "");
    
    // 移除多余的空白字符
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    minified = std::regex_replace(minified, semicolonRegex, ";");
    
    // 移除冒号后的空格
    std::regex colonRegex(R"(\s*:\s*)");
    minified = std::regex_replace(minified, colonRegex, ":");
    
    // 移除大括号前后的空格
    std::regex braceRegex(R"(\s*\{\s*)");
    minified = std::regex_replace(minified, braceRegex, "{");
    
    std::regex closeBraceRegex(R"(\s*\}\s*)");
    minified = std::regex_replace(minified, closeBraceRegex, "}");
    
    // 移除行首行尾的空白
    minified.erase(0, minified.find_first_not_of(" \t\r\n"));
    minified.erase(minified.find_last_not_of(" \t\r\n") + 1);
    
    output = minified;
    
    if (debugMode_) {
        std::cout << "CSS压缩完成，原始长度: " << input.length() 
                  << ", 压缩后长度: " << output.length() << std::endl;
    }
    
    return true;
}

void CSSCompiler::reportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "CSS错误 [" << line << ":" << column << "]: " << message;
    errors_.push_back(ss.str());
    std::cerr << ss.str() << std::endl;
}