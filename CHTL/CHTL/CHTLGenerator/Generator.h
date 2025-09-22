#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>
#include <map>

namespace CHTL {

// The Generator class is responsible for walking the AST and producing output code.
class Generator {
public:
    // Pass the parser's template storage to the generator upon construction.
    Generator(const std::map<std::string, std::unique_ptr<StyleTemplateNode>>& styleTemplates,
              const std::map<std::string, std::unique_ptr<ElementTemplateNode>>& elementTemplates);

    // Generates the final HTML output string from the AST.
    std::string generate(const Program* program);
    // Returns the CSS collected during generation.
    std::string getCss() const;

private:
    // "Visitor" methods for each type of node.
    void visit(const Statement* stmt);
    void visit(const ElementNode* node);
    void visit(const TextNode* node);
    void visit(const StyleBlockNode* node, std::string& inline_style, std::vector<std::string>& auto_classes, std::vector<std::string>& auto_ids);
    void visit(const StylePropertyNode* node, std::string& inline_style);
    void visit(const SelectorRuleNode* node, std::vector<std::string>& auto_classes, std::vector<std::string>& auto_ids);
    void visit(const StyleTemplateUsageNode* node, std::string& inline_style);
    void visit(const ElementTemplateUsageNode* node);


    // Stringstreams to efficiently build the output strings.
    std::stringstream m_html_out;
    std::stringstream m_css_out;

    // References to the parsed templates.
    const std::map<std::string, std::unique_ptr<StyleTemplateNode>>& m_styleTemplates;
    const std::map<std::string, std::unique_ptr<ElementTemplateNode>>& m_elementTemplates;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
