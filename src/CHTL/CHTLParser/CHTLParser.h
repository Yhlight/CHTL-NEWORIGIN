#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/Template/TemplateRegistry.h"
#include <vector>
#include <memory>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    // The result of parsing is now more complex than just one element.
    // We'll have it populate a registry and return the root element.
    std::unique_ptr<ElementNode> parse();
    TemplateRegistry registry; // Make it public for simplicity for now

private:
    std::vector<Token> tokens;
    int current_token_index;

    Token peek();
    Token advance();

    // Parsing helper functions
    void parseTopLevel();
    std::unique_ptr<TemplateDeclarationNode> parseTemplateDeclaration();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<StyleNode> parseStyleBlock();
    Attribute parseAttribute();
};

} // namespace CHTL

#endif // CHTL_PARSER_H
