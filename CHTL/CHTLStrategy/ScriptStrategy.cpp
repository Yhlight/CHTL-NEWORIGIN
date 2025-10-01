#include "ScriptStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/RawScriptNode.h"
#include "../CHTLNode/EnhancedSelectorNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> ScriptStrategy::parse(CHTLParser& parser) {
    parser.consume(TokenType::L_BRACE, "Expect '{' after 'script' keyword.");

    auto scriptNode = std::make_unique<ScriptNode>();

    while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
        if (parser.check(TokenType::RAW_SCRIPT)) {
            Token scriptToken = parser.advance(); // Consume the RAW_SCRIPT token
            scriptNode->addChild(std::make_unique<RawScriptNode>(scriptToken.value));
        } else if (parser.match(TokenType::L_DOUBLE_BRACE)) {
            Token selector = parser.consume(TokenType::IDENTIFIER, "Expect selector inside {{...}}.");
            parser.consume(TokenType::R_DOUBLE_BRACE, "Expect '}}' after selector.");
            scriptNode->addChild(std::make_unique<EnhancedSelectorNode>(selector.value));
        } else {
            // This case handles any other valid JS tokens that might be inside the script block
            // but are not part of our enhanced syntax. We'll add them as raw script.
            // This also acts as a safeguard against infinite loops.
            Token rawToken = parser.advance();
            scriptNode->addChild(std::make_unique<RawScriptNode>(rawToken.value));
        }
    }

    parser.consume(TokenType::R_BRACE, "Expect '}' after script block.");
    return scriptNode;
}

} // namespace CHTL