#include "CHTLParser.h"

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens)
    : context(tokens) {}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    return context.parse();
}

}