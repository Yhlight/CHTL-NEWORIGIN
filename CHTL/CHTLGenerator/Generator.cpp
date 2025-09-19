#include "Generator.h"
#include "../CHTLParser/ExpressionParser.h"
#include "ExpressionEvaluator.h"
#include <stdexcept>

namespace CHTL {

// Helper to convert an EvaluatedValue to a string for CSS
std::string valueToString(const EvaluatedValue& val) {
    switch (val.type) {
        case EvaluatedValue::Type::NUMBER:
            // Remove trailing zeros for clean output
            {
                std::string s = std::to_string(val.number_val);
                s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                s.erase(s.find_last_not_of('.') + 1, std::string::npos);
                return s;
            }
        case EvaluatedValue::Type::STRING:
            return val.string_val;
        case EvaluatedValue::Type::BOOL:
            return val.bool_val ? "true" : "false";
        default:
            return "";
    }
}


Generator::Generator() {}

std::string Generator::generate(const ElementNode& root) {
    output.str(""); // Clear the stream
    output.clear();
    indentLevel = 0;

    // We don't render the virtual "root" element itself, just its children.
    for (const auto& child : root.children) {
        visit(*child);
    }

    return output.str();
}

void Generator::visit(const BaseNode& node) {
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(&node)) {
        visitElement(*elementNode);
    } else if (const auto* textNode = dynamic_cast<const TextNode*>(&node)) {
        visitText(*textNode);
    } else if (const auto* commentNode = dynamic_cast<const CommentNode*>(&node)) {
        visitComment(*commentNode);
    } else {
        // In a real compiler, you might want more robust error handling.
        // For now, we'll just ignore unknown node types.
    }
}

void Generator::visitElement(const ElementNode& node) {
    indent();
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Process and generate style properties
    if (!node.styleProperties.empty()) {
        output << " style=\"";
        std::stringstream style_ss;
        ExpressionEvaluator evaluator;

        for (const auto& styleProp : node.styleProperties) {
            ExpressionParser expressionParser(styleProp.second);
            auto expr = expressionParser.parse();
            if (expr) {
                EvaluatedValue result = evaluator.evaluate(*expr);
                std::string finalValue = valueToString(result);
                // A better, but still simple, unit handling. If the result is a number
                // and the next token is a unit, append it.
                if (result.type == EvaluatedValue::Type::NUMBER) {
                    // This is not robust, but will work for simple "10 px" cases.
                    // A proper implementation requires a more advanced parser/evaluator.
                    for(const auto& token : styleProp.second) {
                        if (token.type == TokenType::UNQUOTED_LITERAL) {
                            finalValue += token.value;
                            break; // only append first unit found
                        }
                    }
                }
                style_ss << styleProp.first << ": " << finalValue << "; ";
            }
        }
        output << style_ss.str() << "\"";
    }


    output << ">" << std::endl;

    indentLevel++;
    for (const auto& child : node.children) {
        visit(*child);
    }
    indentLevel--;

    indent();
    output << "</" << node.tagName << ">" << std::endl;
}

void Generator::visitText(const TextNode& node) {
    indent();
    output << node.content << std::endl;
}

void Generator::visitComment(const CommentNode& node) {
    // Per the spec, only generator comments (#) are rendered.
    if (node.type == CommentType::GENERATOR) {
        indent();
        // The content includes the '#', so we skip it.
        // Also trim leading space if present.
        size_t start = 1;
        if (node.content.length() > 1 && node.content[1] == ' ') {
            start = 2;
        }
        output << "<!-- " << node.content.substr(start) << " -->" << std::endl;
    }
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}

} // namespace CHTL
