#include "CSSCompiler.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/ExpressionParser/ExpressionLexer.h"
#include "CHTL/ExpressionParser/ExpressionParser.h"
#include "CHTL/ExpressionEvaluator/ExpressionEvaluator.h"
#include <regex>
#include <string>

// This is a simplified implementation focusing on the core problem:
// evaluating CHTL expressions inside CSS property values.
// A full implementation would need a proper CSS parser.
std::string CSSCompiler::compile(const std::string& css_with_chtl, Parser& context) {
    std::string final_css = css_with_chtl;
    // Regex to find CSS properties: `property-name: value;`
    // It captures the property name and the value.
    std::regex prop_regex(R"(([\w-]+)\s*:\s*([^;]+);)");

    auto it_begin = std::sregex_iterator(css_with_chtl.begin(), css_with_chtl.end(), prop_regex);
    auto it_end = std::sregex_iterator();

    ExpressionEvaluator evaluator(context);

    // We iterate backwards to avoid invalidating string positions as we replace.
    for (auto it = std::sregex_iterator(css_with_chtl.begin(), css_with_chtl.end(), prop_regex); it != std::sregex_iterator(); ++it) {
        std::smatch match = *it;
        std::string property_name = match[1].str();
        std::string original_value = match[2].str();

        // Don't try to evaluate @-rules or other constructs that aren't simple values.
        if (original_value.find('@') != std::string::npos || original_value.find('{') != std::string::npos) {
            continue;
        }

        try {
            // 1. Tokenize the value string
            ExpressionLexer expr_lexer(original_value);
            auto tokens = expr_lexer.tokenize();

            // If it's just a single literal, no need to evaluate.
            if (tokens.size() <= 1) {
                continue;
            }

            // 2. Parse the tokens into an expression AST
            ExpressionParser expr_parser(tokens);
            auto expr_ast = expr_parser.parse();

            // 3. Evaluate the AST
            std::string evaluated_value = evaluator.evaluate(expr_ast.get());

            // 4. Replace the original CHTL expression with the standard CSS value
            std::string original_declaration = match[0].str();
            std::string new_declaration = property_name + ": " + evaluated_value + ";";
            size_t pos = final_css.find(original_declaration);
            if (pos != std::string::npos) {
                final_css.replace(pos, original_declaration.length(), new_declaration);
            }
        } catch (const std::exception& e) {
            // If parsing or evaluation fails, assume it's just plain CSS and ignore it.
            // A more robust compiler would log this as a warning.
            continue;
        }
    }

    return final_css;
}