#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLManage/TemplateManager.h"
#include "../CHTLManage/ConfigurationManager.h"
#include "../CHTLManage/CJMODManager.h"
#include "../Bridge/SharedContext.h"
#include <memory>
#include <vector>

#include "../CHTLNode/InfoNode.h"

// Forward declarations to avoid circular dependencies
class ParserState;
class ElementNode;

// The Parser class is the main context for the state machine. It holds the
// current state and provides the interface for states to interact with the
// token stream.
class Parser {
public:
    explicit Parser(Lexer& lexer, TemplateManager& tm, ConfigurationManager& cm, std::string source_path = "");
    ~Parser(); // Add destructor declaration

    // The main entry point for parsing. This will populate the `ast` member.
    void parse();

    // Transitions the parser to a new state.
    void setState(std::unique_ptr<ParserState> newState);

    // Static utilities
    static std::string loadFile(const std::string& path);
    static std::map<std::string, std::string> loadCmod(const std::string& path);

    // --- Public interface for ParserState objects ---
    Lexer& lexer;
    Token currentToken;
    Token peekToken;
    Token peekToken2;

    void advanceTokens();
    void expectToken(TokenType type);
    // Expects a keyword, throwing an error if not found. Allows for user-defined aliases.
    void expectKeyword(TokenType type, const std::string& internalName, const std::string& defaultValue);
    // Tries to match a keyword. Returns true and advances if successful, false otherwise.
    bool tryExpectKeyword(TokenType type, const std::string& internalName, const std::string& defaultValue);

    // A context node for states that need to modify a parent, e.g.,
    // a state for parsing attributes needs to know which element to modify.
    ElementNode* contextNode = nullptr;

    // A string to accumulate global CSS rules from style blocks.
    std::string globalStyleContent;

    // The manager for storing and retrieving template definitions.
    TemplateManager& templateManager;

    // The manager for storing and retrieving configuration settings.
    ConfigurationManager& configManager;

    // The manager for loading and interacting with CJMOD extensions.
    CJMODManager cjmodManager;

    // The "Salt Bridge" context for inter-compiler data sharing.
    SharedContext sharedContext;
    int elementIdCounter = 0;

    bool outputHtml5Doctype = false;

    // The stack of active namespaces.
    std::vector<std::string> namespaceStack;
    std::string getCurrentNamespace() const;

    // A list of ampersand rules that are deferred because their base selector
    // (class or id) has not been defined yet.
    std::vector<std::string> deferredAmpersandRules;

    // The AST parsed from the source. It is owned by the parser.
    std::vector<std::unique_ptr<BaseNode>> ast;

    // A pointer to the parsed info node, if one exists.
    std::unique_ptr<InfoNode> infoNode = nullptr;
    // The path of the source file being parsed.
    std::string sourcePath;

    // --- AST Lookup ---
    // Finds the first element node in the parsed AST that matches the given selector.
    // Supports simple selectors: #id, .class, tag
    ElementNode* findElementBySelector(const std::string& selector);

private:
    ElementNode* findElementRecursive(const std::string& selector, const std::vector<std::unique_ptr<BaseNode>>& nodes);
    friend class StatementState; // Allow StatementState to call parseTemplateDefinition
    void parseTemplateDefinition();
    void handleImportStatement();
    void processChtlImport(const std::string& path);
    void processCssImport(const std::string& path);
    std::string getBasePath() const;

public:
    std::unique_ptr<ParserState> currentState;
};
