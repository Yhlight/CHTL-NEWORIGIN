#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLContext.h"
#include <memory>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(ScannedFragments fragments) : m_fragments(std::move(fragments)) {}

std::string CompilerDispatcher::dispatch() {
    // 1. Get the primary CHTL source (with placeholders)
    const std::string& chtl_source = m_fragments.primarySource;

    // 2. Run it through the existing CHTL compiler pipeline.
    CHTLLexer lexer(chtl_source);
    auto context = std::make_shared<CHTLContext>();
    CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTLGenerator generator;
    std::string html_with_placeholders = generator.generate(*ast);

    // 3. For now, we don't do anything with the script blocks.
    // In the future, a CodeMerger component would re-insert the compiled
    // JS into the final HTML.

    // For now, just return the HTML with script placeholders.
    return html_with_placeholders;
}

} // namespace CHTL
