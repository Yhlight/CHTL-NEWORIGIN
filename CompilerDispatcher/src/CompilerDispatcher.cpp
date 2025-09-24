#include "CompilerDispatcher.hpp"
#include "UnifiedScanner.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeMerger.hpp"

using namespace chtl::scanner;
using namespace chtl::core;
using namespace chtl::merger;

namespace chtl::dispatcher {

std::string CompilerDispatcher::compileToHtml(const std::string& source) {
    UnifiedScanner scanner;
    auto frags = scanner.scan(source);

    std::string htmlOut;
    for (const auto& f : frags) {
        if (f.type == FragmentType::Chtl) {
            Lexer lex(f.content);
            auto tokens = lex.tokenize();
            Parser p(tokens);
            auto ast = p.parse();
            (void)ast;
            htmlOut += f.content; // placeholder passthrough
        } else {
            // skip placeholders for now
        }
    }

    CodeMerger merger;
    auto merged = merger.merge(htmlOut, "", "", true, false);
    return merged.html;
}

}
