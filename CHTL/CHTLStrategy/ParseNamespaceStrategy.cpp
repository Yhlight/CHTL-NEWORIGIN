#include "ParseNamespaceStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

void ParseNamespaceStrategy::Execute(Context* context, Parser& parser) {
    parser.ConsumeToken(); // [

    std::string ns_keyword = context->config.KEYWORD_NAMESPACE;
    ns_keyword = ns_keyword.substr(1, ns_keyword.length() - 2);
    if (parser.CurrentToken().lexeme != ns_keyword) return;
    parser.ConsumeToken(); // Namespace

    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) return;
    parser.ConsumeToken(); // ]

    if (parser.CurrentToken().type != TokenType::IDENTIFIER) return;
    std::string namespaceName = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    context->currentNamespace = namespaceName;
    if (context->namespaces.find(namespaceName) == context->namespaces.end()) {
        context->namespaces[namespaceName] = NamespaceData{namespaceName};
    }
}

} // namespace CHTL
