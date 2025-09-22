#pragma once

#include "BaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "StyleBlockNode.h"
#include "StylePropertyNode.h"
#include "TemplateDefinitionNode.h"
#include "TemplateUsageNode.h"
#include "ConfigurationNode.h"
#include "ConfigPropertyNode.h"

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

// A visitor specifically for collecting template definitions
class TemplateCollector : public NodeVisitor {
public:
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override {}
    void visit(StyleBlockNode& node) override;
    void visit(StylePropertyNode& node) override {}
    void visit(TemplateDefinitionNode& node) override;
    void visit(TemplateUsageNode& node) override {}
    // Add empty implementations for the new visit methods
    void visit(ConfigurationNode& node) override {}
    void visit(ConfigPropertyNode& node) override {}

    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> templates;
};


// The Generator class traverses the AST and generates the final HTML output.
class Generator : public NodeVisitor {
public:
    std::string generate(const std::vector<std::shared_ptr<BaseNode>>& nodes);

    // Visitor methods for each node type
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleBlockNode& node) override;
    void visit(StylePropertyNode& node) override;
    void visit(TemplateDefinitionNode& node) override;
    void visit(TemplateUsageNode& node) override;
    // Add empty implementations for the new visit methods
    void visit(ConfigurationNode& node) override {}
    void visit(ConfigPropertyNode& node) override {}


private:
    std::stringstream output_stream;
    int indent_level = 0;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> templates;

    void do_indent();
};
