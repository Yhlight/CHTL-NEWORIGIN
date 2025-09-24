#include "ParseNamespaceStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

void ParseNamespaceStrategy::Execute(Context* context, Parser& parser) {
    // Consume '[' and 'Namespace'
    parser.ConsumeToken();
    parser.ConsumeToken();

    // Expect ]
    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) {
        std::cerr << "Error: Expected ']' after [Namespace." << std::endl;
        return;
    }
    parser.ConsumeToken();

    // Expect namespace name
    if (parser.CurrentToken().type != TokenType::IDENTIFIER) {
        std::cerr << "Error: Expected a name for the namespace." << std::endl;
        return;
    }
    std::string namespaceName = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    // Set the current namespace in the context
    context->currentNamespace = namespaceName;

    // Create the namespace data if it doesn't exist
    if (context->namespaces.find(namespaceName) == context->namespaces.end()) {
        context->namespaces[namespaceName] = NamespaceData{namespaceName};
    }

    // For now, we don't handle {...} blocks for namespaces.
}

} // namespace CHTL
