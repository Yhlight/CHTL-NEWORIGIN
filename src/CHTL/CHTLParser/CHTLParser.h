#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/Node.h"
#include <vector>
#include <memory>

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens);
    std::vector<std::unique_ptr<Node>> parse();

private:
    std::unique_ptr<Node> parseNode();
    std::unique_ptr<Node> parseElementNode();
    std::unique_ptr<Node> parseTextNode();

    std::vector<Token> tokens;
    size_t position = 0;
};

#endif // CHTL_PARSER_H