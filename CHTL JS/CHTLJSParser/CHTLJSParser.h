#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include <vector>
#include <memory>
#include "CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJSNode/CHTLJSNode.h"

class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser() = default;
    
    // 解析Token序列为AST
    bool parse(const std::vector<CHTLJSToken>& tokens, std::unique_ptr<CHTLJSNode>& ast);
    
    // 设置解析选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
private:
    // 解析状态
    std::vector<CHTLJSToken> tokens_;
    size_t currentToken_;
    bool debugMode_;
    
    // 主要解析方法
    bool parseRoot(std::unique_ptr<CHTLJSNode>& root);
    bool parseListen(std::unique_ptr<CHTLJSNode>& listen);
    bool parseAnimate(std::unique_ptr<CHTLJSNode>& animate);
    bool parseRouter(std::unique_ptr<CHTLJSNode>& router);
    bool parseVir(std::unique_ptr<CHTLJSNode>& vir);
    bool parseSelector(std::unique_ptr<CHTLJSNode>& selector);
    bool parseEventBinding(std::unique_ptr<CHTLJSNode>& eventBinding);
    bool parseAnimation(std::unique_ptr<CHTLJSNode>& animation);
    bool parseRoute(std::unique_ptr<CHTLJSNode>& route);
    bool parseVirtualObject(std::unique_ptr<CHTLJSNode>& virtualObject);
    
    // 辅助方法
    CHTLJSToken currentToken() const;
    CHTLJSToken peekToken(size_t offset = 1) const;
    bool match(CHTLJSTokenType type);
    bool match(const std::vector<CHTLJSTokenType>& types);
    void advance();
    bool isAtEnd() const;
    
    // 错误处理
    void reportError(const std::string& message);
    void reportError(const std::string& message, const CHTLJSToken& token);
    
    // 编译状态
    std::vector<std::string> errors_;
};

#endif // CHTL_JS_PARSER_H