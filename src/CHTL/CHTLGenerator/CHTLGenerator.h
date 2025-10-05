#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>

class CHTLGenerator {
public:
    explicit CHTLGenerator(const std::vector<std::unique_ptr<Node>>& ast);
    std::string generate();

private:
    void generateNode(std::stringstream& ss, const Node* node);
    const std::vector<std::unique_ptr<Node>>& ast;
};

#endif // CHTL_GENERATOR_H