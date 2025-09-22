#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "AST.h"
#include <string>
#include <sstream>

class Generator : public NodeVisitor {
public:
    std::string get_html();
    std::string get_css();

    void generate(BaseNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(StyleNode& node) override;
    void visit(CommentNode& node) override;

private:
    std::stringstream html_stream;
    std::stringstream css_stream;
    ElementNode* current_element = nullptr;

    void indent(int level);
};

#endif //CHTL_GENERATOR_H
