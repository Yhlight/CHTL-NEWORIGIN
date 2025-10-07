#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../../Common.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/nodes/ProgramNode.h"
#include "../CHTLNode/nodes/ElementNode.h"
#include "../CHTLNode/nodes/TextNode.h"
#include "../CHTLNode/nodes/CommentNode.h"
#include "../CHTLNode/nodes/StyleNode.h"
#include "../CHTLNode/nodes/ScriptNode.h"
#include "../CHTLNode/nodes/TemplateNode.h"
#include "../CHTLNode/nodes/CustomNode.h"
#include "../CHTLNode/nodes/OriginNode.h"
#include "../CHTLNode/nodes/ImportNode.h"
#include "../CHTLState/CHTLState.h"
#include "../CHTLStrategy/CHTLStrategy.h"

namespace CHTL {

// 解析器配置
struct ParserConfig {
    bool enableComments = true;           // 是否保留注释节点
    bool strictMode = false;              // 严格模式
    bool enableTemplates = true;          // 是否启用模板
    bool enableCustom = true;             // 是否启用自定义
    bool enableCHTLJS = true;             // 是否启用CHTL JS语法
};

// 语法分析器
class CHTLParser {
public:
    explicit CHTLParser(const String& source, const ParserConfig& config = ParserConfig());
    explicit CHTLParser(Vector<Token> tokens, const ParserConfig& config = ParserConfig());
    
    // 解析入口
    SharedPtr<ProgramNode> parse();
    
    // Token操作
    Token getCurrentToken() const;
    Token advance();
    Token peek(int offset = 1) const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    void expect(TokenType type, const String& message);
    Token expectToken(TokenType type, const String& message);
    bool isAtEnd() const;
    
    // 状态管理
    void pushState(SharedPtr<CHTLState> state);
    void popState();
    SharedPtr<CHTLState> getCurrentState() const;
    
    // 策略管理
    void setParseStrategy(SharedPtr<ParseStrategy> strategy);
    SharedPtr<ParseStrategy> getParseStrategy() const;
    
    // 配置
    const ParserConfig& getConfig() const { return config_; }
    
    // 错误处理
    void error(const String& message);
    void warning(const String& message);
    const Vector<String>& getErrors() const { return errors_; }
    const Vector<String>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    
private:
    // 解析方法
    SharedPtr<BaseNode> parseTopLevel();
    SharedPtr<BaseNode> parseStatement();
    SharedPtr<ElementNode> parseElement();
    SharedPtr<TextNode> parseText();
    SharedPtr<CommentNode> parseComment();
    SharedPtr<StyleNode> parseStyle();
    SharedPtr<ScriptNode> parseScript();
    SharedPtr<TemplateNode> parseTemplate();
    SharedPtr<CustomNode> parseCustom();
    SharedPtr<OriginNode> parseOrigin();
    SharedPtr<ImportNode> parseImport();
    
    // 辅助解析方法
    Vector<SharedPtr<BaseNode>> parseBody();
    HashMap<String, String> parseAttributes();
    String parseAttributeValue();
    String parseExpressionValue();  // 新增：解析包含表达式的值
    String parseTextContent();
    String parseBlockContent(TokenType endToken = TokenType::RightBrace);
    
    // 同步错误恢复
    void synchronize();
    
    // 模板引用展开
    SharedPtr<BaseNode> parseElementTemplateReference();
    
    // 元素收集和表达式重新评估
    void collectElementsForRegistration(const SharedPtr<BaseNode>& node);
    void reevaluateExpressionsWithReferences(const SharedPtr<BaseNode>& node);
    
    // 成员变量
    Vector<Token> tokens_;
    size_t current_ = 0;
    ParserConfig config_;
    
    // 状态栈
    Vector<SharedPtr<CHTLState>> stateStack_;
    
    // 当前策略
    SharedPtr<ParseStrategy> currentStrategy_;
    
    // 错误和警告
    Vector<String> errors_;
    Vector<String> warnings_;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
