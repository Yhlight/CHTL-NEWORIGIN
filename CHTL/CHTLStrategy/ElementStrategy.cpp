#include "ElementStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

void ElementStrategy::execute(CHTLParser& parser) {
    // 1. Consume the element's identifier.
    Token identifier = parser.consume(TokenType::IDENTIFIER, "Expected element name.");

    // 2. Open a new ElementNode.
    auto element = std::make_unique<ElementNode>(identifier.value);
    parser.openNode(std::move(element));

    // 3. Consume the opening '{' brace.
    parser.consume(TokenType::L_BRACE, "Expected '{' after element name.");
}

} // namespace CHTL