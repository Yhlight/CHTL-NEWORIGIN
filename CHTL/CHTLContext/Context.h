#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <memory>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "NamespaceData.h"
#include "Configuration.h"

namespace CHTL {

class State;
class Parser;

const std::string GLOBAL_NAMESPACE = "__global__";

class Context {
public:
    Context();
    ~Context();

    void SetState(std::unique_ptr<State> state);
    void Request(Parser& parser);

    // AST building helpers
    void PushNode(Node* node);
    void PopNode();
    Node* GetCurrentParent();
    std::unique_ptr<ElementNode> root;

    // Namespace-aware storage
    std::map<std::string, NamespaceData> namespaces;
    std::string currentNamespace;

    // Global code blocks
    std::string globalCss;

    // Compiler configuration
    Configuration config;

private:
    std::unique_ptr<State> currentState;
    std::stack<Node*> nodeStack;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
