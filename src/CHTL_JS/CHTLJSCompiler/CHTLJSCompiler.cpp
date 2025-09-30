#include "CHTLJSCompiler.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "CJMOD/CJMODScanner.h"
#include "CJMOD/CJMODResultQueue.h"
#include <regex>
#include <algorithm>

// The compile method now integrates the full CJMOD pipeline.
std::string CHTLJSCompiler::compile(const std::string& chtl_js_source, CJMODManager& cjmodManager) {
    std::string processed_source = chtl_js_source;
    int placeholder_index = 0;

    // This loop continues as long as we are successfully finding and replacing
    // CJMOD patterns. Each replacement modifies the source string, so we must
    // re-tokenize and restart the scan to ensure character positions are correct.
    bool replaced_in_pass = true;
    while(replaced_in_pass) {
        replaced_in_pass = false;

        // Tokenize the current version of the source string.
        CHTLJSLexer lexer(processed_source);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();

        for (const auto& module_pair : cjmodManager.getModules()) {
            const auto& module = module_pair.second;

            // Scan from the beginning of the token stream.
            auto result = CJMODScanner::scan(tokens, *(module->syntax_arg), module->keyword, 0);

            if (result.has_value()) {
                // A match was found.

                // 1. Run the module's processing logic to generate the final JS.
                // This will queue the result in the CJMODResultQueue singleton.
                module->processFunc(*(result->matched_arg));

                // 2. Determine the character range of the matched tokens.
                const CHTLJSToken& start_token = tokens[result->start_index];
                const CHTLJSToken& end_token = tokens[result->end_index];
                size_t start_pos = start_token.start_pos;
                size_t length = (end_token.start_pos + end_token.value.length()) - start_pos;

                // 3. Replace the source code with a placeholder.
                std::string placeholder = "__CJMOD_PLACEHOLDER_" + std::to_string(placeholder_index++) + "__";
                processed_source.replace(start_pos, length, placeholder);

                // 4. Mark that we made a change and break to re-tokenize and restart the scan.
                replaced_in_pass = true;
                break;
            }
        }
    }

    // Now, the source has been fully pre-processed.
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(processed_source);

    std::string final_js;
    if (ast) {
        CHTLJSGenerator generator;
        final_js = generator.generate(*ast);
    } else {
        // If the AST is null, the file might contain only CJMOD syntax.
        final_js = processed_source;
    }

    // Substitute the generated CJMOD results back into the final JS.
    auto cjmod_results = CJMODResultQueue::getInstance().popAllResults();
    for (size_t i = 0; i < cjmod_results.size(); ++i) {
        std::string placeholder = "__CJMOD_PLACEHOLDER_" + std::to_string(i) + "__";
        size_t pos = final_js.find(placeholder);
        if (pos != std::string::npos) {
            final_js.replace(pos, placeholder.length(), cjmod_results[i]);
        }
    }

    return final_js;
}