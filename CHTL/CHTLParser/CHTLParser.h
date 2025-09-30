#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<BaseNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    Token peek() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<BaseNode> parseTextNode();
    std::unique_ptr<StyleNode> parseStyleNode();
    std::unique_ptr<ScriptNode> parseScriptNode();
    void parseAttribute(ElementNode* element);
};

} // namespace CHTL

#endif // CHTL_PARSER_H