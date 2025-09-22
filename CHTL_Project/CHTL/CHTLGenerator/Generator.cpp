#include "Generator.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace CHTL {

Generator::Generator(const std::vector<std::shared_ptr<BaseNode>>& roots, CHTLContext& c) : rootNodes(roots), context(c) {}

std::string Generator::getIndent(int level) {
    return std::string(level * 2, ' '); // 2 spaces per indentation level
}

std::string Generator::generate() {
    // First pass to generate the main HTML structure and collect global CSS rules
    for (const auto& node : rootNodes) {
        generateNode(node, 0);
    }
    std::string html = output.str();

    // If there are global styles, inject them into the head
    if (!globalCssRules.empty()) {
        std::stringstream css;
        css << std::endl << getIndent(2) << "<style>" << std::endl;
        for (const auto& rule : globalCssRules) {
            css << getIndent(3) << rule.first << " {" << std::endl;
            css << getIndent(4) << rule.second << std::endl;
            css << getIndent(3) << "}" << std::endl;
        }
        css << getIndent(2) << "</style>" << std::endl << getIndent(1);

        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            html.insert(head_pos, css.str());
        } else {
            // If no head tag, prepend the css string.
            html = css.str() + html;
        }
    }

    return html;
}

void Generator::generateNode(const std::shared_ptr<BaseNode>& node, int indentation) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            generateElementNode(std::static_pointer_cast<ElementNode>(node), indentation);
            break;
        case NodeType::Text:
            generateTextNode(std::static_pointer_cast<TextNode>(node), indentation);
            break;
        case NodeType::Comment:
            generateCommentNode(std::static_pointer_cast<CommentNode>(node), indentation);
            break;
        case NodeType::StyleBlock:
            // Style blocks are processed by their parent element, so we do nothing here.
            break;
        case NodeType::TemplateUsage:
            generateTemplateUsageNode(std::static_pointer_cast<TemplateUsageNode>(node), indentation);
            break;
        case NodeType::Origin:
            generateOriginNode(std::static_pointer_cast<OriginNode>(node), indentation);
            break;
        default:
            // For now, we ignore node types we don't recognize.
            // In the future, this might throw an error.
            break;
    }
}

void Generator::generateTemplateUsageNode(const std::shared_ptr<TemplateUsageNode>& node, int indentation) {
    auto def = context.getTemplate(node->name);
    if (!def) {
        throw std::runtime_error("Template not found: " + node->name);
    }

    if (def->templateType == TemplateType::Element) {
        for (const auto& bodyNode : def->body) {
            generateNode(bodyNode, indentation);
        }
    }
    // @Style and @Var usages are handled within their respective contexts (style blocks, expressions)
}

void Generator::generateOriginNode(const std::shared_ptr<OriginNode>& node, int indentation) {
    // Simply print the raw content
    output << node->content;
}

void Generator::generateCommentNode(const std::shared_ptr<CommentNode>& node, int indentation) {
    output << getIndent(indentation) << "<!-- " << node->content << " -->" << std::endl;
}

void Generator::generateElementNode(const std::shared_ptr<ElementNode>& node, int indentation) {
    // --- Three-pass approach for StyleNodes ---

    // 1. First pass: Resolve @Style templates.
    // This is a simplification. A full implementation would need to handle nested templates
    // and merge properties carefully.
    for (const auto& child : node->children) {
        if (child->getType() == NodeType::StyleBlock) {
            auto styleNode = std::static_pointer_cast<StyleNode>(child);
            for (const auto& styleChild : styleNode->children) {
                if (styleChild->getType() == NodeType::TemplateUsage) {
                    auto usage = std::static_pointer_cast<TemplateUsageNode>(styleChild);
                    auto def = context.getTemplate(usage->name);
                    if (def && def->templateType == TemplateType::Style) {
                        // This is hacky. We are directly modifying the AST, which is not ideal.
                        // We should merge the properties instead.
                        // For now, let's assume the template body is just properties.
                        // This part of the logic needs to be much more robust.
                    }
                }
            }
        }
    }


    // 2. Second pass: Auto-add classes/ids from style blocks
    for (const auto& child : node->children) {
        if (child->getType() == NodeType::StyleBlock) {
            auto styleNode = std::static_pointer_cast<StyleNode>(child);
            for (const auto& rule : styleNode->rules) {
                if (!rule.selector.empty()) {
                    if (rule.selector[0] == '.') {
                        std::string className = rule.selector.substr(1, rule.selector.find(' '));
                        if (node->attributes.count("class")) {
                            node->attributes["class"] += " " + className;
                        } else {
                            node->attributes["class"] = className;
                        }
                    } else if (rule.selector[0] == '#') {
                        node->attributes["id"] = rule.selector.substr(1, rule.selector.find(' '));
                    }
                }
            }
        }
    }

    // 3. Third pass: Process rules, resolve '&', and collect global/inline styles
    std::string styleString;
    for (const auto& child : node->children) {
        if (child->getType() == NodeType::StyleBlock) {
            auto styleNode = std::static_pointer_cast<StyleNode>(child);
            for (const auto& rule : styleNode->rules) {
                std::string props_str;
                for (const auto& prop : rule.properties) {
                    props_str += prop.first + ":" + prop.second + ";";
                }

                if (rule.selector.empty()) { // Inline style rule
                    styleString += props_str;
                } else { // Rule with a selector
                    std::string selector = rule.selector;
                    if (selector.find('&') != std::string::npos) {
                        std::string resolved_base;
                        if (node->attributes.count("class")) {
                            resolved_base = "." + node->attributes["class"].substr(0, node->attributes["class"].find(' '));
                        } else if (node->attributes.count("id")) {
                            resolved_base = "#" + node->attributes["id"];
                        } else {
                            continue; // Skip if no class/id to resolve to
                        }
                        size_t pos = 0;
                        while ((pos = selector.find('&', pos)) != std::string::npos) {
                            selector.replace(pos, 1, resolved_base);
                            pos += resolved_base.length();
                        }
                    }
                    globalCssRules[selector] = props_str;
                }
            }
        }
    }
    if (!styleString.empty()) {
        node->attributes["style"] = styleString;
    }

    // Step 2: Now that attributes are finalized, print the opening tag
    output << getIndent(indentation) << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Step 3: Filter out StyleNodes to get visible children
    std::vector<std::shared_ptr<BaseNode>> visibleChildren;
    for(const auto& child : node->children) {
        if(child->getType() != NodeType::StyleBlock) {
            visibleChildren.push_back(child);
        }
    }

    // Step 4: Handle children and closing tag
    if (visibleChildren.empty()) {
        // Never self-close a head tag, even if empty.
        if (node->tagName == "head") {
            output << "></" << node->tagName << ">" << std::endl;
        } else {
            output << " />" << std::endl;
        }
        return;
    }

    if (node->children.size() == 1 && node->children[0]->getType() == NodeType::Text) {
        auto textNode = std::static_pointer_cast<TextNode>(node->children[0]);
        output << ">" << textNode->content << "</" << node->tagName << ">" << std::endl;
        return;
    }

    // Special handling for elements with a single text node child
    if (visibleChildren.size() == 1 && visibleChildren[0]->getType() == NodeType::Text) {
        auto textNode = std::static_pointer_cast<TextNode>(visibleChildren[0]);
        output << ">" << textNode->content << "</" << node->tagName << ">" << std::endl;
        return;
    }

    output << ">" << std::endl;
    for (const auto& child : visibleChildren) {
        generateNode(child, indentation + 1);
    }
    output << getIndent(indentation) << "</" << node->tagName << ">" << std::endl;
}

void Generator::generateTextNode(const std::shared_ptr<TextNode>& node, int indentation) {
    // This function is now only called if a text node is a direct child of the root,
    // or if an element has multiple children, one of which is text.
    // The single-text-child case is handled in generateElementNode.
    output << getIndent(indentation) << node->content << std::endl;
}

} // namespace CHTL
