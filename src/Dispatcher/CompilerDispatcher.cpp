#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

CompilationResult CompilerDispatcher::compile(
    const std::string& source,
    bool inline_mode,
    const std::string& css_output_filename,
    const std::string& js_output_filename
) {
    // 1. Scan the source to separate CHTL from other languages
    ScannedOutput scanned_output = scanner.scan(source);

    // 2. Initialize CSS and JS content strings
    std::string css_content;
    std::stringstream js_content_stream;

    std::map<std::string, std::string> script_fragments_for_merging;

    // 3. Process all fragments from the scanner
    for (const auto& pair : scanned_output.fragments) {
        const std::string& placeholder = pair.first;
        const CodeFragment& fragment = pair.second;

        if (fragment.type == FragmentType::CSS) {
            css_content += fragment.content + "\n";
        } else if (fragment.type == FragmentType::CHTL_JS) {
            std::string compiled_js = chtl_js_compiler.compile(fragment.content);
            js_content_stream << compiled_js << "\n";
            script_fragments_for_merging[placeholder] = "<script>" + compiled_js + "</script>";
        } else if (fragment.type == FragmentType::JS) {
            js_content_stream << fragment.content << "\n";
            script_fragments_for_merging[placeholder] = "<script>" + fragment.content + "</script>";
        }
    }

    // 4. Compile the main CHTL content
    Lexer lexer(scanned_output.chtl_with_placeholders);
    Parser parser(lexer);
    auto ast = parser.parse();

    // 5. Add any CSS hoisted from local style blocks by the parser
    css_content += parser.globalStyleContent;

    // 6. Generate the HTML.
    Generator generator;
    std::string html_content = generator.generate(
        ast,
        css_content,
        js_content_stream.str(),
        parser.sharedContext,
        parser.outputHtml5Doctype,
        inline_mode,
        css_output_filename,
        js_output_filename
    );

    // 7. If inlining, merge the script placeholders back into the HTML.
    if (inline_mode) {
         html_content = merger.merge(html_content, script_fragments_for_merging);
    }

    CompilationResult result;
    result.html_content = html_content;
    result.css_content = css_content;
    result.js_content = js_content_stream.str();

    return result;
}