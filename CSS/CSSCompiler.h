#ifndef CSS_COMPILER_H
#define CSS_COMPILER_H

#include <string>

class CSSCompiler {
public:
    CSSCompiler();
    ~CSSCompiler() = default;
    
    // 编译CSS代码
    bool compile(const std::string& input, std::string& output);
    
    // 设置编译选项
    void setMinify(bool enabled) { minify_ = enabled; }
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
private:
    // 编译选项
    bool minify_;
    bool debugMode_;
    
    // 编译方法
    bool parseCSS(const std::string& input, std::string& output);
    bool minifyCSS(const std::string& input, std::string& output);
    
    // 错误处理
    void reportError(const std::string& message, size_t line, size_t column);
    
    // 编译状态
    std::vector<std::string> errors_;
};

#endif // CSS_COMPILER_H