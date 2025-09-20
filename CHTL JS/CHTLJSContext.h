#ifndef CHTL_JS_CONTEXT_H
#define CHTL_JS_CONTEXT_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJSParser/CHTLJSParser.h"
#include "CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTLJSNode/CHTLJSNode.h"

class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 编译CHTL JS代码
    bool compile(const std::string& input, std::string& output);
    
    // 设置编译选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setOutputFormat(const std::string& format) { outputFormat_ = format; }
    
private:
    // 编译选项
    bool debugMode_;
    std::string outputFormat_;
    
    // 核心组件
    std::unique_ptr<CHTLJSLexer> lexer_;
    std::unique_ptr<CHTLJSParser> parser_;
    std::unique_ptr<CHTLJSGenerator> generator_;
    
    // 编译流程
    bool tokenize(const std::string& input, std::vector<CHTLJSToken>& tokens);
    bool parse(const std::vector<CHTLJSToken>& tokens, std::unique_ptr<CHTLJSNode>& ast);
    bool generate(const std::unique_ptr<CHTLJSNode>& ast, std::string& output);
    
    // 错误处理
    void reportError(const std::string& message, size_t line, size_t column);
    void reportWarning(const std::string& message, size_t line, size_t column);
    
    // 编译状态
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

#endif // CHTL_JS_CONTEXT_H