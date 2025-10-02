#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>
#include <memory>
namespace CHTL {
class CHTLContext;
class Parser {
public:
    Parser(const std::vector<Token>& tokens, CHTLContext& context);
    std::shared_ptr<BaseNode> parse();
private:
    const std::vector<Token>& tokens_;
    [[maybe_unused]] CHTLContext& context_;
    size_t current_;
    
    Token peek() const;
    Token advance();
    bool isAtEnd() const;
};
} // namespace CHTL
#endif
