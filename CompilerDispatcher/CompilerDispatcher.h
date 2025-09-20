#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <memory>
#include <vector>
#include "Scanner/UnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    // 编译主入口
    bool compile(const std::string& inputFile);
    
    // 设置编译选项
    void setOutputDirectory(const std::string& dir) { outputDir_ = dir; }
    void setInlineMode(bool enabled) { inlineMode_ = enabled; }
    void setInlineCSS(bool enabled) { inlineCSS_ = enabled; }
    void setInlineJS(bool enabled) { inlineJS_ = enabled; }
    void setDefaultStruct(bool enabled) { defaultStruct_ = enabled; }
    
private:
    // 编译选项
    std::string outputDir_;
    bool inlineMode_;
    bool inlineCSS_;
    bool inlineJS_;
    bool defaultStruct_;
    
    // 核心组件
    std::unique_ptr<UnifiedScanner> scanner_;
    std::unique_ptr<CodeMerger> merger_;
    
    // 编译流程
    bool preprocessFile(const std::string& inputFile, std::string& content);
    bool scanCode(const std::string& content, CodeSegments& segments);
    bool compileSegments(const CodeSegments& segments, CompiledCode& compiled);
    bool mergeCode(const CompiledCode& compiled, std::string& finalOutput);
    bool writeOutput(const std::string& output, const std::string& inputFile);
};

#endif // COMPILER_DISPATCHER_H