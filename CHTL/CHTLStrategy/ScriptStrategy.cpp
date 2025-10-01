#include "ScriptStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/ScriptNode.h"
#include <stdexcept>

namespace CHTL {

void ScriptStrategy::execute(CHTLParser& parser) {
    // The state has already consumed the 'script' keyword.
    parser.consume(TokenType::L_BRACE, "Expected '{' after 'script' keyword.");

    // The lexer provides the script content as a single string literal.
    Token content = parser.consume(TokenType::STRING_LITERAL, "Expected script content.");

    parser.consume(TokenType::R_BRACE, "Expected '}' after script content.");

    auto scriptNode = std::make_unique<ScriptNode>(content.value);

    // Attach the completed ScriptNode to the current element.
    ElementNode* currentNode = parser.getCurrentNode();
    if (currentNode) {
        currentNode->setScript(std::move(scriptNode));
    } else {
        throw std::runtime_error("Cannot add script block, no current element node.");
    }
}

} // namespace CHTL