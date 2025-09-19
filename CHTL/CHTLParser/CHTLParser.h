#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <stack>
#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"

namespace CHTL {

class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();

    // 设置输入
    void setTokens(const std::vector<Token>& tokens);
    
    // 解析主入口
    std::shared_ptr<BaseNode> parse();
    
    // 获取解析错误
    const std::vector<std::string>& getErrors() const;
    
    // 清理解析器状态
    void clear();

private:
    std::vector<Token> tokens_;
    size_t currentIndex_;
    std::vector<std::string> errors_;
    std::stack<std::shared_ptr<BaseNode>> nodeStack_;
    
    // 当前Token操作
    Token currentToken() const;
    Token peekToken() const;
    bool hasMoreTokens() const;
    void advance();
    bool match(TokenType type) const;
    bool match(const std::string& value) const;
    
    // 错误处理
    void addError(const std::string& message);
    void addError(const std::string& message, const Token& token);
    
    // 解析方法
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTextNode();
    std::shared_ptr<BaseNode> parseComment();
    std::shared_ptr<BaseNode> parseStyleBlock();
    std::shared_ptr<BaseNode> parseScriptBlock();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseUse();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<BaseNode> element);
    std::pair<std::string, std::string> parseAttribute();
    
    // 样式解析
    void parseStyleProperties(std::shared_ptr<BaseNode> styleNode);
    std::pair<std::string, std::string> parseStyleProperty();
    
    // 局部样式块解析
    void parseLocalStyleBlock(std::shared_ptr<BaseNode> parentElement);
    void parseStyleSelector(std::shared_ptr<BaseNode> styleNode);
    void parseStylePropertyWithOperations(std::shared_ptr<BaseNode> styleNode);
    std::string parsePropertyValue();
    std::string parsePropertyExpression();
    std::string parsePropertyReference();
    std::string parsePropertyConditional();
    std::string parsePropertyArithmetic();
    
    // 选择器解析
    std::string parseClassSelector();
    std::string parseIdSelector();
    std::string parsePseudoSelector();
    std::string parseContextSelector();
    
    // 属性运算解析
    std::string parseArithmeticExpression();
    std::string parseArithmeticTerm();
    std::string parseArithmeticFactor();
    std::string parseArithmeticOperator();
    
    // 属性条件表达式解析
    std::string parseConditionalExpression();
    std::string parseConditionalTerm();
    std::string parseConditionalFactor();
    std::string parseConditionalOperator();
    
    // 引用属性解析
    std::string parseReferenceSelector();
    std::string parseReferenceProperty();
    
    // 辅助方法
    bool isElementStart() const;
    bool isTemplateStart() const;
    bool isCustomStart() const;
    bool isOriginStart() const;
    bool isImportStart() const;
    bool isNamespaceStart() const;
    bool isConfigurationStart() const;
    bool isUseStart() const;
    
    // 跳过空白和注释
    void skipWhitespace();
    void skipComments();
    
    // 解析字符串字面量
    std::string parseStringLiteral();
    std::string parseUnquotedLiteral();
    
    // 解析选择器
    std::string parseSelector();
    
    // 解析表达式
    std::string parseExpression();
    
    // 解析块内容
    void parseBlockContent(std::shared_ptr<BaseNode> parent);
    
    // 解析模板内容
    void parseTemplateContent(std::shared_ptr<BaseNode> templateNode);
    
    // 解析自定义内容
    void parseCustomContent(std::shared_ptr<BaseNode> customNode);
    
    // 解析导入内容
    void parseImportContent(std::shared_ptr<BaseNode> importNode);
    
    // 解析命名空间内容
    void parseNamespaceContent(std::shared_ptr<BaseNode> namespaceNode);
    
    // 解析配置内容
    void parseConfigurationContent(std::shared_ptr<BaseNode> configNode);
};

} // namespace CHTL

#endif // CHTL_PARSER_H