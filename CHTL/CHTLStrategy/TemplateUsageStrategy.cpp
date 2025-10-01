#include "TemplateUsageStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> TemplateUsageStrategy::parse(CHTLParser& parser) {
    Token typeToken = parser.consume(TokenType::IDENTIFIER, "Expect template type (Style, Element, or Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else throw std::runtime_error("Unsupported template usage type: " + typeToken.value);

    Token nameToken = parser.consume(TokenType::IDENTIFIER, "Expect template name.");
    parser.consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
    return std::make_unique<TemplateUsageNode>(type, nameToken.value);
}

} // namespace CHTL