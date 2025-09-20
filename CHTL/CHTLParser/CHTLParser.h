#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLNode/CHTLNode.h"

class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;
    
    // 解析Token序列为AST
    bool parse(const std::vector<Token>& tokens, std::unique_ptr<CHTLNode>& ast);
    
    // 设置解析选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
private:
    // 解析状态
    std::vector<Token> tokens_;
    size_t currentToken_;
    bool debugMode_;
    
    // 主要解析方法
    bool parseRoot(std::unique_ptr<CHTLNode>& root);
    bool parseElement(std::unique_ptr<CHTLNode>& element);
    bool parseTemplate(std::unique_ptr<CHTLNode>& template_);
    bool parseCustom(std::unique_ptr<CHTLNode>& custom);
    bool parseOrigin(std::unique_ptr<CHTLNode>& origin);
    bool parseImport(std::unique_ptr<CHTLNode>& import);
    bool parseConfiguration(std::unique_ptr<CHTLNode>& config);
    bool parseNamespace(std::unique_ptr<CHTLNode>& namespace_);
    bool parseStyle(std::unique_ptr<CHTLNode>& style);
    bool parseScript(std::unique_ptr<CHTLNode>& script);
    bool parseAttributes(std::unique_ptr<CHTLNode>& element);
    bool parseExpression(std::unique_ptr<CHTLNode>& expression);
    
    // 辅助方法
    Token currentToken() const;
    Token peekToken(size_t offset = 1) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    void advance();
    bool isAtEnd() const;
    
    // 错误处理
    void reportError(const std::string& message);
    void reportError(const std::string& message, const Token& token);
    
    // 编译状态
    std::vector<std::string> errors_;
};

#endif // CHTL_PARSER_H