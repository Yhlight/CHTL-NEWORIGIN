#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(const std::vector<Token>& tokens, const std::string& source, CHTLContext& context);

    // The result of parsing is a list of root-level nodes.
    std::vector<std::shared_ptr<BaseNode>> parse();

private:
    const std::vector<Token>& tokens;
    const std::string& source;
    CHTLContext& context;
    size_t position = 0;

    Token peek(size_t offset = 0) const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& error_message);

    // Parsing functions for different CHTL constructs
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<ElementNode> parseElementNode();
    std::shared_ptr<TextNode> parseTextNode();
    std::shared_ptr<StyleNode> parseStyleNode();
    void parseAttributes(std::shared_ptr<ElementNode> node);

    // Template Parsing
    void parseTemplateDefinition();
    void parseCustomDefinition();
    std::shared_ptr<BaseNode> parseUsage(); // General purpose for @Name; or @Name { ... }

    // Other high-level blocks
    std::shared_ptr<OriginNode> parseOriginBlock();
};

} // namespace CHTL
