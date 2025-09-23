#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLState/CHTLState.h"
#include "../CHTLContext/CHTLContext.h"
#include <memory>
#include <stdexcept>

struct OriginNode; // Forward Declaration
struct ImportNode; // Forward Declaration
struct NamespaceNode; // Forward Declaration
struct ConfigNode; // Forward Declaration
struct CustomDefinitionNode; // Forward Declaration
struct ElementSpecializationNode; // Forward Declaration
struct DeleteElementNode; // Forward Declaration
struct ModifyElementNode; // Forward Declaration
struct InsertElementNode; // Forward Declaration

class CHTLParser {
public:
    explicit CHTLParser(CHTLLexer& lexer, CHTLContext& context, const std::string& filePath = "");
    std::shared_ptr<ElementNode> parse();

    // Methods for ExprParser to access the token stream
    void consumeToken();
    const Token& getCurrentToken() const;
    Token peekNextToken();


private:
    CHTLLexer& lexer;
    CHTLContext& context;
    std::string filePath;
    Token currentToken;
    CHTLState state;

    // Token management
    void advanceToken();
    void expect(TokenType type, const std::string& message);

    // Parsing helpers
    std::shared_ptr<BaseNode> parseDirective();
    void parseTemplateDefinition();
    void parseCustomDefinition();
    std::shared_ptr<OriginNode> parseOriginBlock();
    std::map<std::string, std::shared_ptr<BaseExprNode>> parseCssRuleProperties();
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<ElementNode> parseElement();
    void parseBlock(const std::shared_ptr<ElementNode>& element);
    void parseIdentifierInBlock(const std::shared_ptr<ElementNode>& element);
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<BaseNode> parseTemplateUsage();
    std::shared_ptr<ElementSpecializationNode> parseElementSpecializationBlock();
    void parseStyleBlock(const std::shared_ptr<ElementNode>& element);
    std::shared_ptr<TextNode> parseTextNode();
    std::shared_ptr<CommentNode> parseCommentNode();
    std::shared_ptr<ImportNode> parseImportDirective();
    std::shared_ptr<NamespaceNode> parseNamespaceDirective();
    void parseConfigurationDirective();
    void parseExceptClause(const std::shared_ptr<ElementNode>& element);

    // Error handling
    void error(const std::string& message);
};
