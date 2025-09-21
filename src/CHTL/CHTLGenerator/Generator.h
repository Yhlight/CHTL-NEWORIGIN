#ifndef GENERATOR_H
#define GENERATOR_H

#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    std::string Generate(const Program* program);

private:
    void generateNode(const Node* node, std::stringstream& ss);
    void generateElement(const ElementNode* elementNode, std::stringstream& ss);
    void generateText(const TextNode* textNode, std::stringstream& ss);
};

} // namespace CHTL

#endif // GENERATOR_H
