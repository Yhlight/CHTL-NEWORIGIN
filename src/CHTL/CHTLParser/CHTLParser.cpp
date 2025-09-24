#include "CHTLParser.h"
#include "CHTLContext/Context.h"
#include "CHTLState/ParseBaseState.h"

namespace CHTL {

std::unique_ptr<BaseNode> CHTLParser::parse(const std::string& source) {
    CHTLContext context(source);

    while (context.peekToken().type != TokenType::END_OF_FILE) {
        context.request();
    }

    return context.getAST();
}

} // namespace CHTL
