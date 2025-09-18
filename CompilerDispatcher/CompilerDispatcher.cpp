#include "CompilerDispatcher.h"
#include "../CodeMerger/CodeMerger.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLContext.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <memory>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(ScannedFragments fragments) : m_fragments(std::move(fragments)) {}

std::string CompilerDispatcher::dispatch() {
    // 1. Compile the CHTL part to get base HTML with placeholders
    const std::string& chtl_source = m_fragments.primarySource;
    CHTLLexer chtl_lexer(chtl_source);
    auto context = std::make_shared<CHTLContext>();
    CHTLParser chtl_parser(chtl_lexer.getAllTokens(), context);
    auto chtl_ast = chtl_parser.parse();
    CHTLGenerator chtl_generator;
    std::string html_with_placeholders = chtl_generator.generate(*chtl_ast);

    // 2. Compile each script fragment
    std::map<std::string, std::string> compiled_scripts;
    for (const auto& pair : m_fragments.scriptBlocks) {
        const std::string& placeholder = pair.first;
        const std::string& script_source = pair.second;

        CHTL_JS::CHTLJSLexer js_lexer(script_source);
        CHTL_JS::CHTLJSParser js_parser(js_lexer.getAllTokens());
        auto js_ast = js_parser.parse();
        CHTL_JS::CHTLJSGenerator js_generator;
        compiled_scripts[placeholder] = js_generator.generate(js_ast);
    }

    // 3. Merge the compiled JS back into the HTML
    CodeMerger merger;
    std::string final_html = merger.merge(html_with_placeholders, compiled_scripts);

    return final_html;
}

} // namespace CHTL
