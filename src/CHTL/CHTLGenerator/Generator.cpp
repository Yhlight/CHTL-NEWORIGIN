#include "Generator.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/DynamicStyleNode.h"
#include "../CHTLNode/StaticStyleNode.h"
#include <stdexcept>
#include <algorithm>
#include <set>
#include "../ExpressionNode/BinaryOpNode.h"
#include "../ExpressionNode/LiteralNode.h"
#include "../ExpressionNode/PropertyRefNode.h"
#include <stdexcept>

// --- Implementation of the new helper function ---
std::string Generator::generateExpressionString(ExpressionBaseNode* node) {
    if (!node) {
        return "null";
    }

    if (auto* binOp = dynamic_cast<BinaryOpNode*>(node)) {
        std::string left = generateExpressionString(binOp->left.get());
        std::string right = generateExpressionString(binOp->right.get());
        return "(" + left + " " + binOp->op + " " + right + ")";
    }
    if (auto* literal = dynamic_cast<LiteralNode*>(node)) {
        // Here we assume literals in expressions are either numbers or variable names
        // that will be resolved in JS. We might need to add quotes for strings later.
        return literal->value;
    }
    if (auto* propRef = dynamic_cast<PropertyRefNode*>(node)) {
        return "chtl.getProperty('" + propRef->selector + "', '" + propRef->propertyName + "')";
    }

    throw std::runtime_error("Unsupported node type in generateExpressionString.");
}


Generator::Generator() : indentLevel(0) {}

std::string Generator::getIndent() {
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

std::string Generator::generate(
    std::vector<std::unique_ptr<BaseNode>>& roots,
    const std::string& globalCss,
    const std::string& globalJs,
    SharedContext& context,
    bool outputDoctype,
    bool inline_css,
    bool inline_js,
    const std::string& css_output_filename,
    const std::string& js_output_filename
) {
    result.clear();
    indentLevel = 0;
    this->globalCssToInject = globalCss;
    this->globalJsToInject = globalJs;
    this->inlineCss = inline_css;
    this->inlineJs = inline_js;
    this->cssOutputFilename = css_output_filename;
    this->jsOutputFilename = js_output_filename;
    this->mutableContext = &context;

    if (outputDoctype) {
        result += "<!DOCTYPE html>\n";
    }

    for (auto& root : roots) {
        generateNode(root.get());
    }

    return result;
}

void Generator::generateNode(BaseNode* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            generateElement(static_cast<ElementNode*>(node));
            break;
        case NodeType::Text:
            generateText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            generateComment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::Fragment:
            for (auto& child : static_cast<FragmentNode*>(node)->children) {
                generateNode(child.get());
            }
            break;
        case NodeType::Origin:
            generateOrigin(static_cast<const OriginNode*>(node));
            break;
        // Other node types are handled elsewhere or do not generate direct output.
        default:
            break;
    }
}

void Generator::generateOrigin(const OriginNode* node) {
    if (node->type == "Html") {
        // Raw HTML is appended directly to the main result stream.
        append(node->content);
    } else if (node->type == "Style") {
        // Raw CSS is appended to the global CSS content.
        globalCssToInject += node->content + "\n";
    } else if (node->type == "JavaScript") {
        // Raw JS is appended to the global JS content.
        globalJsToInject += node->content + "\n";
    }
    // Note: Other custom origin types are ignored by the generator.
}


void Generator::generateElement(ElementNode* node) {
    if (node->tagName.rfind("chtl_placeholder_", 0) == 0) {
        append(node->tagName + "{}");
        return;
    }
    const std::vector<std::string> selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), node->tagName) != selfClosingTags.end();

    append(getIndent() + "<" + node->tagName);

    // Process attributes first to ensure we have an ID if needed for dynamic styles.
    std::map<std::string, std::string> finalAttributeStrings;
    for (const auto& attrPair : node->attributes) {
        if (attrPair.second) {
            finalAttributeStrings[attrPair.first] = attrPair.second->toString();
        }
    }

    std::string styleString;
    for (const auto& stylePair : node->inlineStyles) {
        if (stylePair.second) {
            if (stylePair.second->getType() == StyleValueType::Dynamic) {
                // Ensure the element has an ID for the runtime to find it.
                if (finalAttributeStrings.find("id") == finalAttributeStrings.end()) {
                    std::string newId = "chtl-id-" + std::to_string(reinterpret_cast<uintptr_t>(node));
                    finalAttributeStrings["id"] = newId;
                }
                const std::string& elementId = finalAttributeStrings.at("id");

                DynamicConditionalBinding binding;
                binding.targetElementId = elementId;
                binding.targetProperty = "style." + stylePair.first;

                auto* dynamicNode = static_cast<DynamicStyleNode*>(stylePair.second.get());
                binding.expression = generateExpressionString(dynamicNode->expressionAst.get());

                mutableContext->dynamicConditionalBindings.push_back(binding);
            } else {
                // It's a static style, so add it to the style string.
                styleString += stylePair.first + ": " + stylePair.second->toString() + "; ";
            }
        }
    }

    // Combine generated styles with any existing style attribute.
    if (!styleString.empty()) {
        if (finalAttributeStrings.count("style")) {
            finalAttributeStrings["style"] = styleString + finalAttributeStrings["style"];
        } else {
            finalAttributeStrings["style"] = styleString;
        }
    }

    // Render all final attributes to the tag.
    for (const auto& [key, val] : finalAttributeStrings) {
        append(" " + key + "=\"" + val + "\"");
    }

    append(">");

    if (isSelfClosing && node->children.empty()) {
        append("\n");
        return;
    }

    if (node->children.size() == 1 && node->children.front()->getType() == NodeType::Text) {
        const auto* textNode = static_cast<const TextNode*>(node->children.front().get());
        append(textNode->text);
    } else if (!node->children.empty() || node->tagName == "head" || node->tagName == "body") {
        append("\n");
        indent();
        if (node->tagName == "head") {
            if (!globalCssToInject.empty()) {
                if (inlineCss) {
                    appendLine("<style>");
                    indent();
                    append(globalCssToInject);
                    outdent();
                    appendLine("</style>");
                } else {
                    appendLine("<link rel=\"stylesheet\" href=\"" + cssOutputFilename + "\">");
                }
            }
        }

        for (auto& child : node->children) {
            generateNode(child.get());
        }

        if (node->tagName == "body") {
            if (!globalJsToInject.empty()) {
                if (inlineJs) {
                    appendLine("<script>");
                    indent();
                    append(globalJsToInject);
                    outdent();
                    appendLine("</script>");
                } else {
                    appendLine("<script src=\"" + jsOutputFilename + "\"></script>");
                }
            }
        }

        outdent();
        append(getIndent());
    }

    append("</" + node->tagName + ">\n");
}

void Generator::generateText(const TextNode* node) {
    appendLine(node->text);
}

void Generator::generateComment(const CommentNode* node) {
    appendLine("<!-- " + node->text + " -->");
}

void Generator::generateNamespace(NamespaceNode* node) {
    for (auto& child : node->children) {
        generateNode(child.get());
    }
}