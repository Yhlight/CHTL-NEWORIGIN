#include "ConfigurationParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ConfigurationNode.h"
#include "../CHTLParser/ParsingUtils.h"

namespace CHTL {

std::shared_ptr<BaseNode> ConfigurationParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Configuration'
    context->advance(); // consume ']'

    auto configNode = std::make_shared<ConfigurationNode>();

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        parseProperties(context, configNode);

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed configuration block
        }
    }

    return configNode;
}

}