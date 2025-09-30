#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens);
    ~CHTLParser();

    std::unique_ptr<BaseNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
};

} // namespace CHTL

#endif // CHTL_PARSER_H