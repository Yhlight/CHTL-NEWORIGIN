#ifndef CHTL_CODE_GENERATOR_H
#define CHTL_CODE_GENERATOR_H

#include "../CHTLNode/AstVisitor.h"
#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>

namespace CHTL {

class Context; // Forward-declare

class CodeGenerator : public AstVisitor {
public:
    std::string Generate(Node* root, Context& context);

    void Visit(ElementNode& node) override;
    void Visit(TextNode& node) override;
    void Visit(CommentNode& node) override;

private:
    std::stringstream output;
};

} // namespace CHTL

#endif // CHTL_CODE_GENERATOR_H
