#include "CHTL/CHTLParser.h"

namespace CHTL {

CHTLParser::CHTLParser() = default;

std::unique_ptr<BaseNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    // TODO: Implement parser
    return std::make_unique<RootNode>();
}

} // namespace CHTL