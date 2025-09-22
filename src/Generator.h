#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "AST.h"
#include <string>
#include <sstream>
#include <map>

class Generator : public NodeVisitor {
public:
    std::string get_html();
    std::string get_css();

    void generate(ProgramNode& root);

    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(StyleNode& node) override;
    void visit(CommentNode& node) override;
    void visit(TemplateStyleNode& node) override;
    void visit(TemplateElementNode& node) override;
    void visit(TemplateVarNode& node) override;
    void visit(StyleUsageNode& node) override;
    void visit(ElementUsageNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(ConfigurationNode& node) override;

private:
    std::stringstream html_stream;
    std::stringstream css_stream;
    ElementNode* current_element = nullptr;

    std::map<std::string, TemplateStyleNode*> style_templates;
    std::map<std::string, TemplateElementNode*> element_templates;
    // std::map<std::string, TemplateVarNode*> var_templates; // For later

    void indent(int level);
};

#endif //CHTL_GENERATOR_H
