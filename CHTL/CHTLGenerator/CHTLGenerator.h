#pragma once

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

// Forward declarations for all node types
class ElementNode;
class TextNode;
class StyleNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class PropertyNode;

class CHTLGenerator {
public:
    void generate(const std::shared_ptr<BaseNode>& node);

    std::string getHtml() const { return html_out.str(); }
    std::string getCss() const { return css_out.str(); }

private:
    void visit(const std::shared_ptr<BaseNode>& node);
    void visit(const std::shared_ptr<ElementNode>& node);
    void visit(const std::shared_ptr<TextNode>& node);
    void visit(const std::shared_ptr<StyleNode>& node);
    void visit(const std::shared_ptr<TemplateNode>& node);
    void visit(const std::shared_ptr<CustomNode>& node);
    void visit(const std::shared_ptr<OriginNode>& node);
    void visit(const std::shared_ptr<ImportNode>& node);
    void visit(const std::shared_ptr<NamespaceNode>& node);
    void visit(const std::shared_ptr<ConfigurationNode>& node);
    void visit(const std::shared_ptr<PropertyNode>& node);

    std::stringstream html_out;
    std::stringstream css_out;
};

}