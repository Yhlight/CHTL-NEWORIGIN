#include "CompilerDispatcher.h"
#include "CHTL/CHTLContext.h"
#include "CHTL JS/CHTLJSContext.h"
#include "CSS/CSSCompiler.h"
#include "JS/JSCompiler.h"
#include <fstream>
#include <iostream>
#include <filesystem>

CompilerDispatcher::CompilerDispatcher() 
    : outputDir_(".")
    , inlineMode_(false)
    , inlineCSS_(false)
    , inlineJS_(false)
    , defaultStruct_(false)
    , scanner_(std::make_unique<UnifiedScanner>())
    , merger_(std::make_unique<CodeMerger>()) {
}

bool CompilerDispatcher::compile(const std::string& inputFile) {
    try {
        std::cout << "开始编译文件: " << inputFile << std::endl;
        
        // 1. 预处理文件
        std::string content;
        if (!preprocessFile(inputFile, content)) {
            return false;
        }
        
        // 2. 扫描代码，分离不同语言的代码片段
        CodeSegments segments;
        if (!scanCode(content, segments)) {
            return false;
        }
        
        // 3. 编译各个代码片段
        CompiledCode compiled;
        if (!compileSegments(segments, compiled)) {
            return false;
        }
        
        // 4. 合并代码
        std::string finalOutput;
        if (!mergeCode(compiled, finalOutput)) {
            return false;
        }
        
        // 5. 写入输出文件
        if (!writeOutput(finalOutput, inputFile)) {
            return false;
        }
        
        std::cout << "编译完成！" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "编译过程中发生错误: " << e.what() << std::endl;
        return false;
    }
}

bool CompilerDispatcher::preprocessFile(const std::string& inputFile, std::string& content) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << inputFile << std::endl;
        return false;
    }
    
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "文件读取成功，大小: " << content.size() << " 字节" << std::endl;
    return true;
}

bool CompilerDispatcher::scanCode(const std::string& content, CodeSegments& segments) {
    std::cout << "开始扫描代码..." << std::endl;
    
    // 使用统一扫描器分离代码
    return scanner_->scan(content, segments);
}

bool CompilerDispatcher::compileSegments(const CodeSegments& segments, CompiledCode& compiled) {
    std::cout << "开始编译代码片段..." << std::endl;
    
    // 编译CHTL代码
    if (!segments.chtlSegments.empty()) {
        CHTLContext chtlContext;
        for (const auto& segment : segments.chtlSegments) {
            std::string compiledSegment;
            if (chtlContext.compile(segment.content, compiledSegment)) {
                compiled.chtlCode += compiledSegment + "\n";
            } else {
                std::cerr << "CHTL代码编译失败: " << segment.line << std::endl;
                return false;
            }
        }
    }
    
    // 编译CHTL JS代码
    if (!segments.chtlJSSegments.empty()) {
        CHTLJSContext chtlJSContext;
        for (const auto& segment : segments.chtlJSSegments) {
            std::string compiledSegment;
            if (chtlJSContext.compile(segment.content, compiledSegment)) {
                compiled.chtlJSCode += compiledSegment + "\n";
            } else {
                std::cerr << "CHTL JS代码编译失败: " << segment.line << std::endl;
                return false;
            }
        }
    }
    
    // 编译CSS代码
    if (!segments.cssSegments.empty()) {
        CSSCompiler cssCompiler;
        for (const auto& segment : segments.cssSegments) {
            std::string compiledSegment;
            if (cssCompiler.compile(segment.content, compiledSegment)) {
                compiled.cssCode += compiledSegment + "\n";
            } else {
                std::cerr << "CSS代码编译失败: " << segment.line << std::endl;
                return false;
            }
        }
    }
    
    // 编译JS代码
    if (!segments.jsSegments.empty()) {
        JSCompiler jsCompiler;
        for (const auto& segment : segments.jsSegments) {
            std::string compiledSegment;
            if (jsCompiler.compile(segment.content, compiledSegment)) {
                compiled.jsCode += compiledSegment + "\n";
            } else {
                std::cerr << "JS代码编译失败: " << segment.line << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

bool CompilerDispatcher::mergeCode(const CompiledCode& compiled, std::string& finalOutput) {
    std::cout << "开始合并代码..." << std::endl;
    
    return merger_->merge(compiled, finalOutput, inlineMode_, inlineCSS_, inlineJS_, defaultStruct_);
}

bool CompilerDispatcher::writeOutput(const std::string& output, const std::string& inputFile) {
    std::cout << "写入输出文件..." << std::endl;
    
    // 创建输出目录
    std::filesystem::create_directories(outputDir_);
    
    // 确定输出文件名
    std::string baseName = std::filesystem::path(inputFile).stem().string();
    std::string outputFile = outputDir_ + "/" + baseName + ".html";
    
    // 写入文件
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "无法创建输出文件: " << outputFile << std::endl;
        return false;
    }
    
    file << output;
    file.close();
    
    std::cout << "输出文件已保存: " << outputFile << std::endl;
    return true;
}