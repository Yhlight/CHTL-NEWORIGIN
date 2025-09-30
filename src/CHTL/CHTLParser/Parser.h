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
    explicit Parser(Lexer& lexer, std::string source_path = "");
    ~Parser(); // Add destructor declaration

    // The main entry point for parsing.
    std::vector<std::unique_ptr<BaseNode>> parse();

    // Transitions the parser to a new state.
    void setState(std::unique_ptr<ParserState> newState);

    // --- Public interface for ParserState objects ---
    Lexer& lexer;
    Token currentToken;
    Token peekToken;
    Token peekToken2;

    void advanceTokens();
    void expectToken(TokenType type);

    // A context node for states that need to modify a parent, e.g.,
    // a state for parsing attributes needs to know which element to modify.
    ElementNode* contextNode = nullptr;

    // A string to accumulate global CSS rules from style blocks.
    std::string globalStyleContent;

    // The manager for storing and retrieving template definitions.
    TemplateManager templateManager;

    // The manager for storing and retrieving configuration settings.
    ConfigurationManager configManager;

    // The manager for loading and interacting with CJMOD extensions.
    CJMODManager cjmodManager;

    // The "Salt Bridge" context for inter-compiler data sharing.
    SharedContext sharedContext;
    int elementIdCounter = 0;

    bool outputHtml5Doctype = false;

    // The stack of active namespaces.
    std::vector<std::string> namespaceStack;
    std::string getCurrentNamespace() const;
    void pushNamespace(const std::string& ns);
    void popNamespace();

    // A list of ampersand rules that are deferred because their base selector
    // (class or id) has not been defined yet.
    std::vector<std::string> deferredAmpersandRules;

    // A pointer to the list of root nodes parsed so far.
    // This allows states to perform lookups on the current AST.
    const std::vector<std::unique_ptr<BaseNode>>* parsedNodes = nullptr;

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

public:
    std::unique_ptr<ParserState> currentState;
};