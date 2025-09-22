#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <sstream>

namespace CHTL {

// Forward declare node types to avoid circular dependencies with the nodes themselves.
class BaseNode;
class ProgramNode;
class ElementNode;
class TextNode;
class StyleNode;
class TemplateStyleNode;

class Generator {
public:
    std::string generate(const BaseNode& root);

    void visit(const ProgramNode& node);
    void visit(const ElementNode& node);
    void visit(const TextNode& node);
    void visit(const StyleNode& node);
    void visit(const TemplateStyleNode& node);

private:
    std::stringstream ss;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
