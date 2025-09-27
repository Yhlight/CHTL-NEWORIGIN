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

    // 2. Initialize CSS string and map for JS fragments
    std::string globalCss;
    std::map<std::string, std::string> compiled_js_fragments;

    // 3. Process all fragments from the scanner
    for (const auto& pair : scanned_output.fragments) {
        const std::string& placeholder = pair.first;
        const CodeFragment& fragment = pair.second;

        if (fragment.type == FragmentType::CSS) {
            // Append content of global style blocks to the CSS string
            globalCss += fragment.content;
        } else if (fragment.type == FragmentType::CHTL_JS) {
            // Compile CHTL JS fragments and prepare them for merging
            std::string compiled_js = chtl_js_compiler.compile(fragment.content);
            compiled_js_fragments[placeholder] = "<script>" + compiled_js + "</script>";
        } else if (fragment.type == FragmentType::JS) {
            // For now, just wrap plain JS in script tags.
            // In the future, this could be passed to a JS minifier/bundler.
            compiled_js_fragments[placeholder] = "<script>" + fragment.content + "</script>";
        }
    }

    // 4. Compile the main CHTL content
    Lexer lexer(scanned_output.chtl_with_placeholders);
    Parser parser(lexer);
    auto ast = parser.parse();

    // 5. Add any CSS hoisted from local style blocks by the parser
    globalCss += parser.globalStyleContent;

    // 6. Generate the initial HTML from the CHTL AST, passing the combined global CSS
    Generator generator;
    std::string htmlOutput = generator.generate(ast, globalCss, parser.sharedContext, parser.outputHtml5Doctype);

    // 7. Use the CodeMerger to re-insert the compiled JavaScript fragments
    return merger.merge(htmlOutput, compiled_js_fragments);
}