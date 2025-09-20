#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include "Scanner/UnifiedScanner.h"

class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger() = default;
    
    // 合并编译后的代码
    bool merge(const CompiledCode& compiled, std::string& finalOutput, 
               bool inlineMode, bool inlineCSS, bool inlineJS, bool defaultStruct);
    
private:
    // 生成HTML结构
    std::string generateHTMLStructure(const CompiledCode& compiled, 
                                     bool inlineMode, bool inlineCSS, bool inlineJS, 
                                     bool defaultStruct);
    
    // 生成CSS部分
    std::string generateCSS(const CompiledCode& compiled, bool inlineMode);
    
    // 生成JavaScript部分
    std::string generateJS(const CompiledCode& compiled, bool inlineMode);
    
    // 生成默认HTML结构
    std::string generateDefaultHTMLStructure();
    
    // 内联CSS
    std::string inlineCSS(const std::string& css);
    
    // 内联JavaScript
    std::string inlineJS(const std::string& js);
    
    // 生成独立的CSS文件
    bool generateCSSFile(const std::string& css, const std::string& baseName);
    
    // 生成独立的JS文件
    bool generateJSFile(const std::string& js, const std::string& baseName);
};

#endif // CODE_MERGER_H