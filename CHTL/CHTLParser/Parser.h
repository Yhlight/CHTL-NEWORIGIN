#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(std::vector<Token> tokens);

    // The main entry point, returns the root of the AST.
    // CHTL files can have multiple root-level elements, so we wrap them in a virtual "root" element.
    std::unique_ptr<ElementNode> parse();

private:
    const std::vector<Token> tokens;
    size_t current = 0;

    // --- Token Helpers ---
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // --- Parsing Rules ---
    std::unique_ptr<BaseNode> parseDeclaration();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<CommentNode> parseCommentNode(TokenType type);
    void parseAttributes(ElementNode& node);
    void parseStyleBlock(ElementNode& node);
};

} // namespace CHTL
