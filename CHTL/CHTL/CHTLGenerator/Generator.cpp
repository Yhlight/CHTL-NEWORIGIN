#include "Generator.h"
#include <stdexcept>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

namespace CHTL {

Generator::Generator(const std::map<std::string, std::unique_ptr<StyleTemplateNode>>& styleTemplates,
                     const std::map<std::string, std::unique_ptr<ElementTemplateNode>>& elementTemplates)
    : m_styleTemplates(styleTemplates), m_elementTemplates(elementTemplates) {}

std::string Generator::generate(const Program* program) {
    m_html_out.str("");
    m_html_out.clear();
    m_css_out.str("");
    m_css_out.clear();

    for (const auto& stmt : program->statements) {
        visit(stmt.get());
    }
    return m_html_out.str();
}

void Generator::visit(const Statement* stmt) {
    if (auto p = dynamic_cast<const ElementNode*>(stmt)) {
        visit(p);
    } else if (auto p = dynamic_cast<const TextNode*>(stmt)) {
        visit(p);
    } else if (auto p = dynamic_cast<const ElementTemplateUsageNode*>(stmt)) {
        visit(p);
    }
    // Other node types are either handled elsewhere (like StylePropertyNode) or produce no direct output (like definitions).
}

std::string Generator::getCss() const {
    return m_css_out.str();
}

void Generator::visit(const StylePropertyNode* node, std::string& inline_style) {
    // Append to the inline style string
    if (!inline_style.empty()) {
        inline_style += "; ";
    }
    inline_style += node->property.literal + ": " + node->value;
}

void Generator::visit(const SelectorRuleNode* node, std::vector<std::string>& auto_classes, std::vector<std::string>& auto_ids) {
    // Add selector name to the auto-attachment list
    if (node->token.type == TokenType::DOT) {
        auto_classes.push_back(node->name.literal);
    } else if (node->token.type == TokenType::HASH) {
        auto_ids.push_back(node->name.literal);
    }

    // Generate the CSS rule and add it to the global CSS output
    m_css_out << node->getSelector() << " { ";
    for (size_t i = 0; i < node->properties.size(); ++i) {
        const auto& prop = node->properties[i];
        m_css_out << prop->property.literal << ": " << prop->value << "; ";
    }
    m_css_out << "}\n";
}


void Generator::visit(const StyleTemplateUsageNode* node, std::string& inline_style) {
    std::string key = node->fromNamespace.literal.empty()
                        ? node->name.literal
                        : node->fromNamespace.literal + "." + node->name.literal;

    auto it = m_styleTemplates.find(key);
    if (it == m_styleTemplates.end()) {
        // Handle error: Template not found.
        return;
    }

    // Create a mutable copy of the template's properties
    std::vector<std::unique_ptr<StylePropertyNode>> final_properties;
    const auto& templateNode = it->second;
    for (const auto& prop : templateNode->properties) {
        auto newProp = std::make_unique<StylePropertyNode>();
        newProp->property = prop->property;
        newProp->value = prop->value;
        final_properties.push_back(std::move(newProp));
    }

    // Apply specializations from the usage block
    for (const auto& stmt : node->body) {
        if (auto p = dynamic_cast<const StylePropertyNode*>(stmt.get())) {
            // This is a property assignment, find the original and update it
            bool found = false;
            for (auto& final_prop : final_properties) {
                if (final_prop->property.literal == p->property.literal) {
                    final_prop->value = p->value;
                    found = true;
                    break;
                }
            }
            if (!found) { // Or add it if it's a new property
                final_properties.push_back(std::make_unique<StylePropertyNode>(*p));
            }
        } else if (auto d = dynamic_cast<const DeleteStatementNode*>(stmt.get())) {
            // This is a delete statement, remove the property
            final_properties.erase(
                std::remove_if(final_properties.begin(), final_properties.end(),
                    [&](const auto& prop) {
                        return prop->property.literal == d->target.literal;
                    }),
                final_properties.end()
            );
        }
    }

    // Generate the inline style from the final list of properties
    for (const auto& prop : final_properties) {
        if (!prop->value.empty()) { // Don't generate valueless properties
            visit(prop.get(), inline_style);
        }
    }
}

void Generator::visit(const ElementTemplateUsageNode* node) {
    std::string key = node->fromNamespace.literal.empty()
                        ? node->name.literal
                        : node->fromNamespace.literal + "." + node->name.literal;

    auto it = m_elementTemplates.find(key);
    if (it != m_elementTemplates.end()) {
        // Found the template, now expand its body
        const auto& templateNode = it->second;
        for (const auto& stmt : templateNode->body) {
            visit(stmt.get());
        }
    } else {
        // Handle error: Template not found.
    }
}

void Generator::visit(const StyleBlockNode* node, std::string& inline_style, std::vector<std::string>& auto_classes, std::vector<std::string>& auto_ids) {
    for (const auto& stmt : node->body) {
        if (auto p = dynamic_cast<const StylePropertyNode*>(stmt.get())) {
            visit(p, inline_style);
        } else if (auto p = dynamic_cast<const SelectorRuleNode*>(stmt.get())) {
            visit(p, auto_classes, auto_ids);
        } else if (auto p = dynamic_cast<const StyleTemplateUsageNode*>(stmt.get())) {
            visit(p, inline_style);
        }
    }
}

void Generator::visit(const ElementNode* node) {
    std::string inline_style;
    std::vector<std::string> auto_classes;
    std::vector<std::string> auto_ids;

    // Pre-process the style block to gather inline styles, global rules, and auto-attachment names
    if (node->styleBlock) {
        visit(node->styleBlock.get(), inline_style, auto_classes, auto_ids);
    }

    // Generate the opening tag.
    m_html_out << "<" << node->tokenLiteral();

    // Process attributes and auto-attachment classes/ids together
    std::string existing_class;
    std::set<std::string> final_classes;

    // Collect auto-attached classes first
    for(const auto& cls : auto_classes) final_classes.insert(cls);

    // Process explicit attributes, extracting any existing class
    for (const auto& attr : node->attributes) {
        if (attr->token.literal == "class") {
            // Found an existing class attribute, add its contents to the set
            std::stringstream ss(attr->value.literal);
            std::string cls;
            while (ss >> cls) {
                final_classes.insert(cls);
            }
        } else {
            // Output other attributes directly
            m_html_out << " " << attr->token.literal << "=\"" << attr->value.literal << "\"";
        }
    }

    // Write the merged class attribute
    if (!final_classes.empty()) {
        m_html_out << " class=\"";
        for (auto it = final_classes.begin(); it != final_classes.end(); ++it) {
            m_html_out << *it << (std::next(it) != final_classes.end() ? " " : "");
        }
        m_html_out << "\"";
    }

    // Handle auto-attachment of id, overwriting if one exists
    if (!auto_ids.empty()) {
        m_html_out << " id=\"" << auto_ids[0] << "\"";
    }

    // Generate collected inline style
    if (!inline_style.empty()) {
        m_html_out << " style=\"" << inline_style << "\"";
    }

    m_html_out << ">";

    // Recursively visit all child nodes.
    for (const auto& child : node->children) {
        visit(child.get());
    }

    // Generate the closing tag.
    m_html_out << "</" << node->tokenLiteral() << ">";
}

void Generator::visit(const TextNode* node) {
    // Simply output the literal value of the text node.
    m_html_out << node->value.literal;
}

} // namespace CHTL
