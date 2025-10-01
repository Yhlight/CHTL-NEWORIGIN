#include "CommentStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

void CommentStrategy::execute(CHTLParser& parser) {
    // The state has already identified a GENERATOR_COMMENT token.
    Token commentToken = parser.consume(TokenType::GENERATOR_COMMENT, "Expected generator comment.");

    // Create a new CommentNode.
    auto commentNode = std::make_unique<CommentNode>(commentToken.value);

    // Add the new node to the current parent.
    parser.addNode(std::move(commentNode));
}

} // namespace CHTL