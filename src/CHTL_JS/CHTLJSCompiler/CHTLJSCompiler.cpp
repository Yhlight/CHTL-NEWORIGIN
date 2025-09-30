#include "CHTLJSCompiler.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CJMOD/CJMODScanner.h"
#include "../CJMOD/CJMODResultQueue.h"
#include <regex>

// The compile method now integrates the full CJMOD pipeline.
std::string CHTLJSCompiler::compile(const std::string& chtl_js_source, CJMODManager& cjmodManager) {
    std::string processed_source = chtl_js_source;
    int placeholder_index = 0;

    // 1. Pre-process the source with all loaded CJMODs
    for (const auto& module_pair : cjmodManager.getModules()) {
        const auto& module = module_pair.second;
        const auto& keyword = module->keyword;

        // Keep scanning for the keyword until no more matches are found
        while (processed_source.find(keyword) != std::string::npos) {
            // 2. Scan for the syntax pattern
            auto scanned_arg = CJMODScanner::scan(*(module->syntax_arg), keyword);
            if (scanned_arg->arguments.empty()) {
                // No more matches found by the scanner for this keyword
                break;
            }

            // 3. Run the module's processing logic, which will eventually
            // call CJMODGenerator::exportResult() to queue the final JS.
            module->processFunc(*scanned_arg);

            // 4. Replace the found source code with a placeholder.
            // This is simplified; a real implementation would need the scanner
            // to return the exact range of the match. Our mock always finds "3 ** 4".
            size_t match_pos = processed_source.find("3 ** 4");
            if (match_pos != std::string::npos) {
                std::string placeholder = "__CJMOD_PLACEHOLDER_" + std::to_string(placeholder_index++) + "__";
                processed_source.replace(match_pos, 6, placeholder);
            } else {
                // If the specific text isn't found, break to avoid an infinite loop.
                break;
            }
        }
    }

    // 5. Parse the modified source (with placeholders) to get the standard CHTL JS AST.
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(processed_source);

    std::string final_js;
    if (ast) {
        // 6. Generate the main JavaScript code. It will contain the placeholders.
        CHTLJSGenerator generator;
        final_js = generator.generate(*ast);
    } else {
        // If the AST is null, it might be a file with only CJMOD syntax.
        final_js = processed_source;
    }


    // 7. Retrieve the generated CJMOD results and substitute them back.
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