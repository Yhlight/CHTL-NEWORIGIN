#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/CHTLNode.h"

class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 编译CHTL代码
    bool compile(const std::string& input, std::string& output);
    
    // 设置编译选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setOutputFormat(const std::string& format) { outputFormat_ = format; }
    
private:
    // 编译选项
    bool debugMode_;
    std::string outputFormat_;
    
    // 核心组件
    std::unique_ptr<CHTLLexer> lexer_;
    std::unique_ptr<CHTLParser> parser_;
    std::unique_ptr<CHTLGenerator> generator_;
    
    // 编译流程
    bool tokenize(const std::string& input, std::vector<Token>& tokens);
    bool parse(const std::vector<Token>& tokens, std::unique_ptr<CHTLNode>& ast);
    bool generate(const std::unique_ptr<CHTLNode>& ast, std::string& output);
    
    // 错误处理
    void reportError(const std::string& message, size_t line, size_t column);
    void reportWarning(const std::string& message, size_t line, size_t column);
    
    // 编译状态
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

#endif // CHTL_CONTEXT_H