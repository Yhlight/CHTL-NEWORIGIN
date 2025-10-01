#include "ScriptStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> ScriptStrategy::parse(CHTLParser& parser) {
    parser.consume(TokenType::L_BRACE, "Expect '{' after 'script' keyword.");
    Token content = parser.consume(TokenType::STRING_LITERAL, "Expect script content.");
    parser.consume(TokenType::R_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content.value);
}

} // namespace CHTL