#pragma once

#include "CHTLState/State.h"
#include "CHTLStrategy/Strategy.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleBlockNode.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @class CHTLContext
 * @brief Manages the state and strategy of the parser.
 */
class CHTLContext {
public:
    explicit CHTLContext(const std::string& source);

    // State management
    void setState(std::unique_ptr<State> newState);
    void request();

    // Strategy management
    void setStrategy(std::unique_ptr<Strategy> newStrategy);
    void executeStrategy();

    // Token management
    Token consumeToken();
    const Token& peekToken(size_t offset = 0) const;

    // AST management
    void createRootNode(const std::string& tagName);
    ElementNode* getCurrentNode() const;
    void pushNode(ElementNode* node);
    void popNode();
    std::unique_ptr<BaseNode> getAST();

    // Style block management
    void setCurrentStyleBlock(StyleBlockNode* styleBlock);
    StyleBlockNode* getCurrentStyleBlock() const;

private:
    std::unique_ptr<State> _currentState;
    std::unique_ptr<Strategy> _currentStrategy;
    CHTLLexer _lexer;
    std::vector<Token> _tokens;
    size_t _currentTokenIndex = 0;

    std::unique_ptr<BaseNode> _ast;
    std::vector<ElementNode*> _nodeStack;
    StyleBlockNode* _currentStyleBlock = nullptr; // Pointer to the current style block
};

} // namespace CHTL
