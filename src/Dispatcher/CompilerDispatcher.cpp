#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <string>
#include <vector>
#include <memory>

std::string CompilerDispatcher::compile(const std::string& source) {
    // 1. Scan the source to separate CHTL from other languages
    ScannedOutput scanned_output = scanner.scan(source);

    std::string globalCss;
    SharedContext sharedContext;
    bool outputDoctype = false;

    // 2. Compile the main CHTL content
    Lexer lexer(scanned_output.chtl_with_placeholders);
    Parser parser(lexer);
    auto ast = parser.parse();
    globalCss += parser.globalStyleContent;
    if (parser.outputHtml5Doctype) {
        outputDoctype = true;
    }

    // 3. Collect CSS from style blocks
    for (const auto& pair : scanned_output.fragments) {
        if (pair.second.type == FragmentType::CSS) {
            globalCss += pair.second.content;
        }
    }

    // 4. Generate the initial HTML from the CHTL AST
    Generator generator;
    std::string htmlOutput = generator.generate(ast, globalCss, sharedContext, outputDoctype);

    // 5. Use the CodeMerger to re-insert the original fragments
    return merger.merge(htmlOutput, scanned_output.fragments);
}