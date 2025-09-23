#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLManage/TemplateManager.h"
#include <memory>
#include <vector>

// Forward declarations to avoid circular dependencies
class ParserState;
class ElementNode;
struct Token; // Forward declare Token for the snapshot

struct ParserStateSnapshot {
    Token currentToken;
    Token peekToken;
    size_t lexerPosition;
};

// The Parser class is the main context for the state machine. It holds the
// current state and provides the interface for states to interact with the
// token stream.
class Parser {
public:
    explicit Parser(Lexer& lexer);
    ~Parser();

    ParserStateSnapshot saveState();
    void restoreState(const ParserStateSnapshot& snapshot);

    // The main entry point for parsing.
    std::vector<std::unique_ptr<BaseNode>> parse();

    // Transitions the parser to a new state.
    void setState(std::unique_ptr<ParserState> newState);

    // --- Public interface for ParserState objects ---
    Lexer& lexer;
    Token currentToken;
    Token peekToken;

    void advanceTokens();
    void expectToken(TokenType type);

    // A context node for states that need to modify a parent, e.g.,
    // a state for parsing attributes needs to know which element to modify.
    ElementNode* contextNode = nullptr;

    // A string to accumulate global CSS rules from style blocks.
    std::string globalStyleContent;

    // The manager for storing and retrieving template definitions.
    TemplateManager templateManager;

    bool outputHtml5Doctype = false;

    // The stack of active namespaces.
    std::vector<std::string> namespaceStack;
    std::string getCurrentNamespace() const;
    // --- End public interface for states ---

private:
    std::unique_ptr<ParserState> currentState;
};
