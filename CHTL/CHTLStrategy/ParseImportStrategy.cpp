#include "ParseImportStrategy.h"
#include "../CHTLContext/Context.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

void ParseImportStrategy::Execute(Context* context, Parser& parser) {
    parser.ConsumeToken(); // [

    std::string import_keyword = context->config.KEYWORD_IMPORT;
    import_keyword = import_keyword.substr(1, import_keyword.length() - 2);
    if (parser.CurrentToken().lexeme != import_keyword) return;
    parser.ConsumeToken(); // Import

    if (parser.CurrentToken().type != TokenType::RIGHT_BRACKET) return;
    parser.ConsumeToken(); // ]

    if (parser.CurrentToken().type != TokenType::AT) return;
    parser.ConsumeToken();
    if (parser.CurrentToken().lexeme != "Chtl") return;
    parser.ConsumeToken();

    std::string from_keyword = context->config.KEYWORD_FROM;
    if (parser.CurrentToken().lexeme != from_keyword) return;
    parser.ConsumeToken();

    if (parser.CurrentToken().type != TokenType::STRING) return;
    std::string filepath = parser.CurrentToken().lexeme;
    parser.ConsumeToken();

    parser.loader.ImportFile(filepath);
}

} // namespace CHTL
