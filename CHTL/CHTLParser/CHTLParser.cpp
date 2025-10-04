#include "CHTLParser.h"
#include "../CHTLContext/ConfigurationManager.h"

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens, std::shared_ptr<ConfigurationManager> configManager)
    : context(tokens, configManager) {}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    return context.parse();
}

}