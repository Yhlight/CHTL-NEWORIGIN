#include "Context.h"
#include "../CHTLState/State.h"
#include "../CHTLParser/Parser.h" // Needed for the Request method
#include <iostream>

namespace CHTL {

Context::Context() {
    this->root = std::make_unique<ElementNode>("__ROOT__");
    this->nodeStack.push(this->root.get());
    this->currentNamespace = GLOBAL_NAMESPACE;
    this->namespaces[GLOBAL_NAMESPACE] = NamespaceData{GLOBAL_NAMESPACE};
}

Context::~Context() = default;

void Context::SetState(std::unique_ptr<State> state) {
    if (state) {
        this->currentState = std::move(state);
    }
}

void Context::Request(Parser& parser) {
    if (this->currentState) {
        // The state's Handle method will now get tokens from the parser,
        // which is passed through the context. This is a bit indirect.
        // A better way is for Handle to take both context and parser.
        // Let's modify State.h for that.
        this->currentState->Handle(this, parser);
    }
}

void Context::PushNode(Node* node) {
    nodeStack.push(node);
}

void Context::PopNode() {
    if (nodeStack.size() > 1) { // Don't pop the root
        nodeStack.pop();
    }
}

Node* Context::GetCurrentParent() {
    return nodeStack.top();
}

} // namespace CHTL
