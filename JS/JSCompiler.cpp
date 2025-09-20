#include "JSCompiler.h"
#include <iostream>
#include <sstream>
#include <regex>

JSCompiler::JSCompiler() 
    : minify_(false)
    , debugMode_(false) {
}

bool JSCompiler::compile(const std::string& input, std::string& output) {
    std::cout << "开始编译JavaScript代码..." << std::endl;
    
    try {
        if (minify_) {
            return minifyJS(input, output);
        } else {
            return parseJS(input, output);
        }
    } catch (const std::exception& e) {
        std::cerr << "JavaScript编译过程中发生错误: " << e.what() << std::endl;
        return false;
    }
}

bool JSCompiler::parseJS(const std::string& input, std::string& output) {
    // 简单的JavaScript解析和验证
    // 这里可以使用ANTLR4或其他JS解析库
    
    if (debugMode_) {
        std::cout << "解析JavaScript代码，长度: " << input.length() << std::endl;
    }
    
    // 基本的JavaScript语法检查
    std::regex functionRegex(R"(function\s+\w+\s*\([^)]*\)\s*\{[^}]*\})");
    std::sregex_iterator begin(input.begin(), input.end(), functionRegex);
    std::sregex_iterator end;
    
    size_t functionCount = 0;
    for (auto it = begin; it != end; ++it) {
        functionCount++;
        if (debugMode_) {
            std::cout << "找到JavaScript函数 " << functionCount << std::endl;
        }
    }
    
    if (debugMode_) {
        std::cout << "共找到 " << functionCount << " 个JavaScript函数" << std::endl;
    }
    
    // 直接输出，这里可以添加更复杂的处理
    output = input;
    return true;
}

bool JSCompiler::minifyJS(const std::string& input, std::string& output) {
    // 简单的JavaScript压缩
    std::string minified = input;
    
    // 移除单行注释
    std::regex singleLineComment(R"(\/\/[^\r\n]*)");
    minified = std::regex_replace(minified, singleLineComment, "");
    
    // 移除多行注释
    std::regex multiLineComment(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)");
    minified = std::regex_replace(minified, multiLineComment, "");
    
    // 移除多余的空白字符
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    minified = std::regex_replace(minified, semicolonRegex, ";");
    
    // 移除逗号后的空格
    std::regex commaRegex(R"(\s*,\s*)");
    minified = std::regex_replace(minified, commaRegex, ",");
    
    // 移除大括号前后的空格
    std::regex openBraceRegex(R"(\s*\{\s*)");
    minified = std::regex_replace(minified, openBraceRegex, "{");
    
    std::regex closeBraceRegex(R"(\s*\}\s*)");
    minified = std::regex_replace(minified, closeBraceRegex, "}");
    
    // 移除小括号前后的空格
    std::regex openParenRegex(R"(\s*\(\s*)");
    minified = std::regex_replace(minified, openParenRegex, "(");
    
    std::regex closeParenRegex(R"(\s*\)\s*)");
    minified = std::regex_replace(minified, closeParenRegex, ")");
    
    // 移除行首行尾的空白
    minified.erase(0, minified.find_first_not_of(" \t\r\n"));
    minified.erase(minified.find_last_not_of(" \t\r\n") + 1);
    
    output = minified;
    
    if (debugMode_) {
        std::cout << "JavaScript压缩完成，原始长度: " << input.length() 
                  << ", 压缩后长度: " << output.length() << std::endl;
    }
    
    return true;
}

void JSCompiler::reportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "JavaScript错误 [" << line << ":" << column << "]: " << message;
    errors_.push_back(ss.str());
    std::cerr << ss.str() << std::endl;
}