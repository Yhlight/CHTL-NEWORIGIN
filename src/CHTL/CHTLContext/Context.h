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

// Define a common interface for nodes that can contain style properties
class StyleContainerNode {
public:
    virtual ~StyleContainerNode() = default;
    std::vector<std::unique_ptr<BaseNode>> rules;
};

// Update AST nodes to inherit from the new interface
#include "CHTLNode/SelectorBlockNode.h"
// We will need to modify StyleBlockNode and SelectorBlockNode to use this.
// For now, let's just add the stack to the context.

class CHTLContext {
public:
    explicit CHTLContext(const std::string& source);

    void setState(std::unique_ptr<State> newState);
    void request();
    void setStrategy(std::unique_ptr<Strategy> newStrategy);
    void executeStrategy();
    Token consumeToken();
    const Token& peekToken(size_t offset = 0) const;

    // AST management
    void createRootNode(const std::string& tagName);
    ElementNode* getCurrentNode() const;
    void pushNode(ElementNode* node);
    void popNode();
    std::unique_ptr<BaseNode> getAST();

    // Style block/container management
    void pushStyleContainer(BaseNode* container);
    void popStyleContainer();
    BaseNode* getCurrentStyleContainer() const;


private:
    std::unique_ptr<State> _currentState;
    std::unique_ptr<Strategy> _currentStrategy;
    CHTLLexer _lexer;
    std::vector<Token> _tokens;
    size_t _currentTokenIndex = 0;

    std::unique_ptr<BaseNode> _ast;
    std::vector<ElementNode*> _nodeStack;
    std::vector<BaseNode*> _styleContainerStack;
};

} // namespace CHTL
