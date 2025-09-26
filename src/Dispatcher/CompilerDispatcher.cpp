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

    // 3. Compile other fragments (CHTL_JS, CSS, etc.)
    std::map<std::string, std::string> compiled_fragments;
    for (const auto& pair : scanned_output.fragments) {
        const std::string& placeholder = pair.first;
        const CodeFragment& fragment = pair.second;
        if (fragment.type == FragmentType::CSS) {
            globalCss += fragment.content;
        } else if (fragment.type == FragmentType::CHTL_JS) {
            compiled_fragments[placeholder] = chtl_js_compiler.compile(fragment.content);
        }
    }

    // 4. Generate the initial HTML from the CHTL AST
    Generator generator;
    std::string htmlOutput = generator.generate(ast, globalCss, sharedContext, outputDoctype);

    // 5. Use the CodeMerger to re-insert any compiled JS fragments (currently none)
    std::string finalHtml = merger.merge(htmlOutput, compiled_fragments);

    // 6. Inject the global CSS into the head of the document
    if (!globalCss.empty()) {
        std::string style_tag = "<style>" + globalCss + "</style>";
        size_t head_pos = finalHtml.find("</head>");
        if (head_pos != std::string::npos) {
            finalHtml.insert(head_pos, style_tag);
        } else {
            // Fallback if no </head> is found
            finalHtml = style_tag + finalHtml;
        }
    }

    return finalHtml;
}