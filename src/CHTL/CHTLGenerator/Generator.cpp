#include "Generator.h"
#include "../Util/StyleUtil.h" // For styleValueToString
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include <stdexcept>
#include <algorithm> // For std::find

Generator::Generator() : indentLevel(0) {}

std::string Generator::getIndent() {
    // Use 2 spaces for each indentation level for clean output.
    return std::string(indentLevel * 2, ' ');
}

void Generator::indent() {
    indentLevel++;
}

void Generator::outdent() {
    if (indentLevel > 0) {
        indentLevel--;
    }
}

void Generator::append(const std::string& str) {
    result += str;
}

void Generator::appendLine(const std::string& str) {
    result += getIndent() + str + "\n";
}

std::string Generator::generate(const std::vector<std::unique_ptr<BaseNode>>& roots, const std::string& globalCss, bool outputDoctype) {
    result.clear();
    indentLevel = 0;
    this->globalCssToInject = globalCss; // Store the CSS for later injection.

    if (outputDoctype) {
        result += "<!DOCTYPE html>\n";
    }

    for (const auto& root : roots) {
        generateNode(root.get());
    }
    return result;
}

void Generator::generateNode(const BaseNode* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            generateElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            generateText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            generateComment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::Fragment:
            // For a fragment, just generate its children directly.
            for (const auto& child : static_cast<const FragmentNode*>(node)->children) {
                generateNode(child.get());
            }
            break;
        case NodeType::Origin:
            // For an origin node, just print its raw content.
            append(static_cast<const OriginNode*>(node)->content);
            break;
        default:
            // This should not be reached if the parser is correct.
            throw std::runtime_error("Unknown node type encountered in generator.");
    }
}

void Generator::generateElement(const ElementNode* node) {
    // A list of HTML tags that are self-closing.
    const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };

    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), node->tagName) != selfClosingTags.end();

    append(getIndent() + "<" + node->tagName);

    // Process attributes, converting them to strings.
    auto finalAttributes = node->attributes;

    // Evaluate conditional styles and merge them into the attributes map.
    for (const auto& conditionalBlock : node->conditionalStyles) {
        if (conditionalBlock.condition.type != StyleValue::BOOL) {
            throw std::runtime_error("Conditional style block has a non-boolean condition at generation time.");
        }
        if (conditionalBlock.condition.bool_val) {
            for (const auto& stylePair : conditionalBlock.styles) {
                finalAttributes[stylePair.first] = stylePair.second;
            }
            break;
        }
    }

    // Consolidate all style-related properties into a single style string.
    std::string styleString;
    std::vector<std::string> keysToRemove;
    for (auto const& [key, val] : finalAttributes) {
        // A simple heuristic: if a value is numeric or it's not a standard, non-style attribute,
        // treat it as a style. This is imperfect but works for this case.
        if (val.type == StyleValue::NUMERIC || (key != "id" && key != "class" && key != "href" && key != "src")) {
            styleString += key + ": " + styleValueToString(val) + "; ";
            keysToRemove.push_back(key);
        }
    }

    // Remove the style properties from the attributes map so they aren't rendered twice.
    for (const auto& key : keysToRemove) {
        finalAttributes.erase(key);
    }

    // Add the consolidated style string as a single 'style' attribute.
    if (!styleString.empty()) {
        finalAttributes["style"] = {StyleValue::STRING, 0.0, "", styleString};
    }

    // Append attributes to the opening tag.
    if (!finalAttributes.empty()) {
        for (const auto& attr : finalAttributes) {
            append(" " + attr.first + "=\"" + styleValueToString(attr.second) + "\"");
        }
    }

    append(">");

    // Self-closing tags do not have content or a closing tag.
    if (isSelfClosing && node->children.empty()) {
        append("\n");
        return;
    }

    // Handle children and potential global style injection
    if (!node->children.empty() || (node->tagName == "head" && !globalCssToInject.empty())) {
        append("\n");
        indent();

        // 1. Generate all children from the AST
        for (const auto& child : node->children) {
            generateNode(child.get());
        }

        // 2. If this is the head tag, inject the collected global styles
        if (node->tagName == "head" && !globalCssToInject.empty()) {
            appendLine("<style>");
            indent();
            result += globalCssToInject; // Append raw CSS content
            outdent();
            appendLine("</style>");
        }

        outdent();
        append(getIndent());
    }

    append("</" + node->tagName + ">\n");
}

void Generator::generateText(const TextNode* node) {
    // Text is output with indentation relative to its parent element.
    appendLine(node->text);
}

void Generator::generateComment(const CommentNode* node) {
    // CHTL '#' comments are rendered as HTML comments.
    appendLine("<!-- " + node->text + " -->");
}
