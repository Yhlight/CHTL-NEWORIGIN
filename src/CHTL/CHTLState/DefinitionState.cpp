#include "DefinitionState.h"
#include "DefaultState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <stdexcept>

DefinitionState::DefinitionState(const std::string& category) : category(category) {}

void DefinitionState::handle(CHTLParser& parser, Token token) {
    // We've already consumed [Template] or [Custom].
    // The token passed should be the AT_SIGN.
    if (token.type != TokenType::AT_SIGN) {
        throw std::runtime_error("Expected @ after " + category);
    }

    Token typeToken = parser.consume();
    if (typeToken.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected template/custom type (e.g., Style, Element) after @");
    }
    std::string type = typeToken.value;

    Token nameToken = parser.consume();
    if (nameToken.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected template/custom name after type");
    }
    std::string name = nameToken.value;

    auto defNode = std::make_unique<TemplateDefinitionNode>(category, type, name);
    auto* defNodePtr = defNode.get();
    parser.addNode(std::move(defNode));

    // Next token should be the opening brace
    Token brace = parser.consume();
    if (brace.type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' to open definition block");
    }

    parser.openScope(defNodePtr);
    parser.setState(std::make_unique<TagState>());
}