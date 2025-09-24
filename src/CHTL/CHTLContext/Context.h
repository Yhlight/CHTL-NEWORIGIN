#pragma once

#include "CHTLState/State.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @class CHTLContext
 * @brief Manages the state of the parser and provides a shared context.
 */
class CHTLContext {
public:
    explicit CHTLContext(const std::string& source);

    void setState(std::unique_ptr<State> newState);
    void request();

    // Token management
    Token consumeToken();
    const Token& peekToken(size_t offset = 0) const;

    // AST management
    void createRootNode(const std::string& tagName);
    ElementNode* getCurrentNode() const;
    void pushNode(ElementNode* node);
    void popNode();
    std::unique_ptr<BaseNode> getAST(); // New method to retrieve the final AST

private:
    std::unique_ptr<State> _currentState;
    CHTLLexer _lexer;
    std::vector<Token> _tokens;
    size_t _currentTokenIndex = 0;

    std::unique_ptr<BaseNode> _ast;
    std::vector<ElementNode*> _nodeStack; // Stack to manage nested elements
};

} // namespace CHTL
