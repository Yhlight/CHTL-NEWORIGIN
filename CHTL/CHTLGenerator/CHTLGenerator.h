#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/GenerationContext.h"
#include "../CHTLParser/ParsingUtils.h" // For parseCondition
#include <string>
#include <memory>
#include <sstream>
#include <map>
#include <vector>

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
class RuleNode;
class UseNode;
class TemplateUsageNode;
class DeleteNode;
class InsertNode;
class ScriptNode;
class AnimateNode;
class IfNode; // Added IfNode
class SaltBridge;

class CHTLGenerator {
public:
    void generate(const std::shared_ptr<BaseNode>& node, const GenerationContext& context, SaltBridge* bridge);

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
    void visit(const std::shared_ptr<RuleNode>& node);
    void visit(const std::shared_ptr<UseNode>& node);
    void visit(const std::shared_ptr<TemplateUsageNode>& node);
    void visit(const std::shared_ptr<DeleteNode>& node);
    void visit(const std::shared_ptr<InsertNode>& node);
    void visit(const std::shared_ptr<ScriptNode>& node);
    void visit(const std::shared_ptr<AnimateNode>& node);
    void visit(const std::shared_ptr<IfNode>& node); // Added IfNode visit method

    std::stringstream html_out;
    std::stringstream css_out;
    const GenerationContext* context;
    SaltBridge* salt_bridge;
    std::vector<std::shared_ptr<ElementNode>> element_stack;

    void collectStyleProperties(const std::shared_ptr<TemplateNode>& tNode, std::vector<std::pair<std::string, std::string>>& properties);
    void generateConditionalCss(const std::vector<std::shared_ptr<IfNode>>& chain);
};

}