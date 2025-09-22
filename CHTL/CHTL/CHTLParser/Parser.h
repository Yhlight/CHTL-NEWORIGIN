#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLLoader/Loader.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace CHTL {

class Parser {
public:
    // Symbol tables to store template definitions.
    std::map<std::string, std::unique_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<ElementTemplateNode>> elementTemplates;

    // Constructor: Initializes the parser with a reference to the lexer.
    Parser(Lexer& lexer);

    // The main entry point for parsing. It returns the root of the AST.
    std::unique_ptr<Program> parseProgram();

    // Returns any errors encountered during parsing.
    const std::vector<std::string>& errors() const { return m_errors; }

private:
    // Advances the tokens.
    void nextToken();

    // Parsing methods for different language constructs.
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ImportNode> parseImportStatement();
    std::unique_ptr<NamespaceNode> parseNamespaceNode();
    std::unique_ptr<Statement> parseTemplateDefinition(bool isCustom);
    std::unique_ptr<StyleTemplateNode> parseStyleTemplateDefinition();
    std::unique_ptr<ElementTemplateNode> parseElementTemplateDefinition();
    std::unique_ptr<StyleTemplateUsageNode> parseStyleTemplateUsage();
    std::unique_ptr<ElementTemplateUsageNode> parseElementTemplateUsage();
    std::unique_ptr<DeleteStatementNode> parseDeleteStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<AttributeNode> parseAttributeNode();
    std::unique_ptr<StyleBlockNode> parseStyleBlockNode();
    std::unique_ptr<StylePropertyNode> parseStylePropertyNode();
    std::unique_ptr<SelectorRuleNode> parseSelectorRuleNode();

    // Helper methods for token validation.
    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);

    Lexer& m_lexer;
    Loader m_loader;
    Token m_currentToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
    std::vector<std::string> m_namespaceStack;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
