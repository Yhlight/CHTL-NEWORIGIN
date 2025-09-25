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

std::string Generator::generate(const std::vector<std::unique_ptr<BaseNode>>& roots, const std::string& globalCss, const SharedContext& context, bool outputDoctype) {
    result.clear();
    indentLevel = 0;
    this->globalCssToInject = globalCss;

    if (outputDoctype) {
        result += "<!DOCTYPE html>\n";
    }

    for (const auto& root : roots) {
        generateNode(root.get());
    }

    generateRuntimeScript(context);

    return result;
}

void Generator::generateNode(const BaseNode* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            generateElement(const_cast<ElementNode*>(static_cast<const ElementNode*>(node)));
            break;
        case NodeType::Text:
            generateText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            generateComment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::Fragment:
            for (const auto& child : static_cast<const FragmentNode*>(node)->children) {
                generateNode(child.get());
            }
            break;
        case NodeType::Origin:
            append(static_cast<const OriginNode*>(node)->content);
            break;
        case NodeType::Script:
            generateScript(static_cast<const ScriptNode*>(node));
            break;
        default:
            break;
    }
}

void Generator::generateElement(ElementNode* node) {
    const std::vector<std::string> selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), node->tagName) != selfClosingTags.end();

    append(getIndent() + "<" + node->tagName);

    auto finalAttributes = node->attributes;
    std::string styleString;

    // Filter out responsive values from inline styles, they are handled by the runtime
    for (const auto& stylePair : node->inlineStyles) {
        if (stylePair.second.type != StyleValue::RESPONSIVE) {
            styleString += stylePair.first + ": " + styleValueToString(stylePair.second) + "; ";
        }
    }

    if (!styleString.empty()) {
        if (finalAttributes.count("style")) {
            finalAttributes["style"].string_val = styleString + finalAttributes["style"].string_val;
        } else {
            finalAttributes["style"] = {StyleValue::STRING, 0.0, "", styleString};
        }
    }

    // Filter out responsive values from attributes
    for (auto it = finalAttributes.begin(); it != finalAttributes.end(); ) {
        if (it->second.type == StyleValue::RESPONSIVE) {
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

    if (!node->children.empty() || (node->tagName == "head" && !globalCssToInject.empty())) {
        append("\n");
        indent();
        for (const auto& child : node->children) {
            generateNode(child.get());
        }
        if (node->tagName == "head" && !globalCssToInject.empty()) {
            appendLine("<style>");
            indent();
            result += globalCssToInject;
            outdent();
            appendLine("</style>");
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
    if (node->isGeneratorComment) {
        // The lexer strips the leading space, so we add one back for formatting.
        appendLine("<!-- " + node->text + " -->");
    }
}

void Generator::generateScript(const ScriptNode* node) {
    append(getIndent() + "<script>");
    std::string scriptContent;
    for (const auto& child : node->children) {
        switch(child->getType()) {
            case NodeType::RawScript:
                scriptContent += static_cast<const RawScriptNode*>(child.get())->content;
                break;
            case NodeType::EnhancedSelector:
                scriptContent += "document.querySelector('" + static_cast<const EnhancedSelectorNode*>(child.get())->selector + "')";
                break;
            case NodeType::Origin:
                 scriptContent += static_cast<const OriginNode*>(child.get())->content;
                 break;
            default:
                break;
        }
    }
    append(scriptContent);
    append("</script>\n");
}

void Generator::generateRuntimeScript(const SharedContext& context) {
    if (context.responsiveBindings.empty()) {
        return;
    }

    appendLine("<script>");
    indent();
    appendLine("const __chtl = {");
    indent();
    appendLine("bindings: {},");
    appendLine("registerBinding(variable, elementId, property, unit) {");
    indent();
    appendLine("if (!this.bindings[variable]) { this.bindings[variable] = []; }");
    appendLine("this.bindings[variable].push({ elementId, property, unit });");
    outdent();
    appendLine("},");
    appendLine("updateDOM(variable, value) {");
    indent();
    appendLine("if (!this.bindings[variable]) return;");
    appendLine("this.bindings[variable].forEach(binding => {");
    indent();
    appendLine("const element = document.getElementById(binding.elementId);");
    appendLine("if (element) {");
    indent();
    appendLine("const finalValue = typeof value === 'number' ? value + binding.unit : value;");
    appendLine("if (binding.property.startsWith('style.')) {");
    indent();
    appendLine("element.style[binding.property.substring(6)] = finalValue;");
    outdent();
    appendLine("} else {");
    indent();
    appendLine("element.setAttribute(binding.property, finalValue);");
    outdent();
    appendLine("}");
    outdent();
    appendLine("}");
    outdent();
    appendLine("});");
    outdent();
    appendLine("}");
    outdent();
    appendLine("};");

    std::set<std::string> uniqueVarNames;
    for(const auto& pair : context.responsiveBindings) {
        uniqueVarNames.insert(pair.first);
        for(const auto& binding : pair.second) {
            std::stringstream ss;
            ss << "__chtl.registerBinding('" << pair.first << "', '" << binding.elementId << "', '" << binding.property << "', '" << binding.unit << "');";
            appendLine(ss.str());
        }
    }

    for (const auto& varName : uniqueVarNames) {
        appendLine("let " + varName + "_val;");
        appendLine("Object.defineProperty(window, '" + varName + "', {");
        indent();
        appendLine("get() { return " + varName + "_val; },");
        appendLine("set(newValue) {");
        indent();
        appendLine(varName + "_val = newValue;");
        appendLine("__chtl.updateDOM('" + varName + "', newValue);");
        outdent();
        appendLine("}");
        outdent();
        appendLine("});");
    }

    outdent();
    appendLine("</script>");
}