#pragma once

#include "CHTLCommon.h"
#include "Token.h"
#include "ASTNode.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <memory>
#include <vector>
#include <stack>

namespace CHTL {

// CHTL Parser - 解析CHTL语法并构建AST
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();

    // 解析CHTL代码，返回AST根节点
    std::shared_ptr<ASTNode> parse(const std::string& source);
    std::shared_ptr<ASTNode> parse(const std::vector<Token>& tokens);

    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t)> handler);
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }

    // 配置选项
    void setStrictMode(bool strict) { strictMode = strict; }
    void setDebugMode(bool debug) { debugMode = debug; }

private:
    // 解析方法
    std::shared_ptr<ASTNode> parseDocument();
    std::shared_ptr<ASTNode> parseTopLevelElement();
    std::shared_ptr<ASTNode> parseElement();
    std::shared_ptr<ASTNode> parseTextNode();
    std::shared_ptr<ASTNode> parseComment();
    std::shared_ptr<ASTNode> parseGeneratorComment();
    std::shared_ptr<ASTNode> parseLocalStyleBlock();
    std::shared_ptr<ASTNode> parseScriptBlock();
    std::shared_ptr<ASTNode> parseTemplate();
    std::shared_ptr<ASTNode> parseCustom();
    std::shared_ptr<ASTNode> parseOrigin();
    std::shared_ptr<ASTNode> parseImport();
    std::shared_ptr<ASTNode> parseNamespace();
    std::shared_ptr<ASTNode> parseConfiguration();

    // 属性解析
    std::map<std::string, std::string> parseAttributes();
    std::pair<std::string, std::string> parseAttribute();

    // 样式块解析
    std::map<std::string, std::string> parseStyleProperties();
    std::pair<std::string, std::string> parseStyleProperty();

    // 辅助方法
    bool match(TokenType type);
    bool match(const std::string& value);
    bool match(TokenType type, const std::string& value);
    Token consume(TokenType type);
    Token consume(TokenType type, const std::string& value);
    Token peek() const;
    Token peek(size_t offset) const;
    Token advance();
    bool isAtEnd() const;
    void skipWhitespace();
    
    // 错误报告
    void error(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
    void synchronize();

    // 验证方法
    bool isValidElementName(const std::string& name);
    bool isValidAttributeName(const std::string& name);
    bool isValidCSSProperty(const std::string& property);

    // 状态管理
    std::vector<Token> tokens;
    size_t current;
    std::vector<std::string> errors;
    std::function<void(const std::string&, size_t)> errorHandler;
    
    // 配置
    bool strictMode;
    bool debugMode;

    // 解析状态
    std::stack<std::shared_ptr<ASTNode>> nodeStack;
    std::map<std::string, std::shared_ptr<ASTNode>> templates;
    std::map<std::string, std::shared_ptr<ASTNode>> customs;
    std::vector<std::string> namespaces;

    // 调试辅助
    void debugPrint(const std::string& message);
    void debugPrintToken(const Token& token);
};

// 解析器工具函数
namespace ParserUtils {
    // 验证CHTL语法
    bool validateCHTLSyntax(const std::string& source);
    
    // 提取文档信息
    struct DocumentInfo {
        std::string title;
        std::vector<std::string> imports;
        std::vector<std::string> namespaces;
        std::vector<std::string> templates;
        std::vector<std::string> customs;
        bool hasRootElement;
    };
    
    DocumentInfo extractDocumentInfo(const std::string& source);
    
    // 语法检查
    bool checkElementNesting(const std::string& source);
    bool checkAttributeSyntax(const std::string& source);
    bool checkStyleSyntax(const std::string& source);
    bool checkScriptSyntax(const std::string& source);
}

} // namespace CHTL