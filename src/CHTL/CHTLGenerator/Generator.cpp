#include "Generator.h"
#include "../Util/StyleUtil.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/RawScriptNode.h"
#include "../CHTLNode/EnhancedSelectorNode.h"
#include <stdexcept>
#include <algorithm>
#include <set>

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
    bool inline_mode,
    const std::string& css_output_filename,
    const std::string& js_output_filename
) {
    result.clear();
    indentLevel = 0;
    this->globalCssToInject = globalCss;
    this->globalJsToInject = globalJs;
    this->inlineMode = inline_mode;
    this->cssOutputFilename = css_output_filename;
    this->jsOutputFilename = js_output_filename;
    this->mutableContext = &context;

    if (outputDoctype) {
        result += "<!DOCTYPE html>\n";
    }

    for (auto& root : roots) {
        generateNode(root.get());
    }

    generateRuntimeScript(context); // This will now be part of the global JS.

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
            append(static_cast<const OriginNode*>(node)->content);
            break;
        case NodeType::Script:
            // Script content is now handled by the dispatcher and passed in globalJs
            break;
        case NodeType::Conditional:
            generateConditional(static_cast<ConditionalNode*>(node));
            break;
        case NodeType::Namespace:
            generateNamespace(static_cast<NamespaceNode*>(node));
            break;
        default:
            break;
    }
}

void Generator::generateConditional(ConditionalNode* node) {
    bool isDynamic = false;
    for (const auto& conditionalCase : node->cases) {
        if (conditionalCase.condition.type == StyleValue::DYNAMIC_CONDITIONAL) {
            isDynamic = true;
            break;
        }
    }

    if (isDynamic) {
        for (auto& conditionalCase : node->cases) {
            if (conditionalCase.condition.type == StyleValue::DYNAMIC_CONDITIONAL) {
                std::string wrapperId = "chtl-dyn-render-" + std::to_string(reinterpret_cast<uintptr_t>(&conditionalCase));
                appendLine("<div id=\"" + wrapperId + "\">");
                indent();
                for (const auto& child : conditionalCase.children) {
                    generateNode(child.get());
                }
                outdent();
                appendLine("</div>");

                DynamicRenderingBinding binding;
                binding.elementId = wrapperId;
                binding.expression = conditionalCase.condition.dynamic_expr;
                mutableContext->dynamicRenderingBindings.push_back(binding);
            }
        }
    } else {
        for (const auto& conditionalCase : node->cases) {
            if (conditionalCase.condition.type == StyleValue::BOOL && conditionalCase.condition.bool_val) {
                for (const auto& child : conditionalCase.children) {
                    generateNode(child.get());
                }
                break;
            }
        }
    }
}


void Generator::generateElement(ElementNode* node) {
    if (node->tagName.rfind("chtl_placeholder_", 0) == 0) {
        // Placeholders are handled by the dispatcher's merger now.
        // We just need to make sure they are present in the output.
        append(node->tagName + "{}");
        return;
    }
    const std::vector<std::string> selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), node->tagName) != selfClosingTags.end();

    append(getIndent() + "<" + node->tagName);

    auto finalAttributes = node->attributes;
    std::string styleString;

    for (const auto& stylePair : node->inlineStyles) {
        if (stylePair.second.type != StyleValue::RESPONSIVE && stylePair.second.type != StyleValue::DYNAMIC_CONDITIONAL) {
            styleString += stylePair.first + ": " + styleValueToString(stylePair.second) + "; ";
        }
    }

    if (!styleString.empty()) {
        if (finalAttributes.count("style")) {
            finalAttributes["style"].string_val = styleString + finalAttributes["style"].string_val;
        } else {
            finalAttributes["style"] = StyleValue(styleString);
        }
    }

    for (auto it = finalAttributes.begin(); it != finalAttributes.end(); ) {
        if (it->second.type == StyleValue::RESPONSIVE || it->second.type == StyleValue::DYNAMIC_CONDITIONAL) {
            it = finalAttributes.erase(it);
        } else {
            ++it;
        }
    }

    for (auto const& [key, val] : finalAttributes) {
        append(" " + key + "=\"" + styleValueToString(val) + "\"");
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
                if (inlineMode) {
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
                if (inlineMode) {
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

void Generator::generateScript(const ScriptNode* node) {
    // This is now handled by the dispatcher, which passes the combined script
    // to the generate method. This function is effectively deprecated.
}

void Generator::generateNamespace(NamespaceNode* node) {
    // Namespaces are for organization and don't produce output themselves,
    // so we just generate their children.
    for (auto& child : node->children) {
        generateNode(child.get());
    }
}

void Generator::generateRuntimeScript(const SharedContext& context) {
    // This logic is now part of the global JS content passed to the main generate method.
    // The dispatcher will concatenate this with other script content.
}