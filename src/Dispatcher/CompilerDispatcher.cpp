#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTL/CHTLNode/OriginNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <set>
#include <regex>

namespace {
// Generates the JavaScript runtime environment required for CHTL's dynamic features.
std::string generateRuntimeScript(const SharedContext& context) {
    if (context.responsiveBindings.empty() && context.dynamicConditionalBindings.empty() && context.dynamicRenderingBindings.empty()) {
        return "";
    }

    std::stringstream runtime_script;
    std::set<std::string> responsive_vars;

    runtime_script << R"JS(
const __chtl = {
    bindings: {},
    registerBinding: function(varName, elementId, property, unit) {
        if (!this.bindings[varName]) {
            this.bindings[varName] = [];
        }
        this.bindings[varName].push({ elementId, property, unit });
    },
    updateDOM: function(varName, value) {
        if (this.bindings[varName]) {
            this.bindings[varName].forEach(binding => {
                const element = document.getElementById(binding.elementId);
                if (element) {
                    if (binding.property.startsWith('style.')) {
                        const styleProp = binding.property.substring(6);
                        element.style[styleProp] = value + binding.unit;
                    } else {
                        element.setAttribute(binding.property, value);
                    }
                }
            });
        }
    }
};
)JS";

    for (const auto& pair : context.responsiveBindings) {
        const std::string& varName = pair.first;
        responsive_vars.insert(varName);
        for (const auto& binding : pair.second) {
            runtime_script << "__chtl.registerBinding('" << varName << "', '" << binding.elementId << "', '" << binding.property << "', '" << binding.unit << "');\n";
        }
    }

    for (const auto& varName : responsive_vars) {
        runtime_script << R"JS(
Object.defineProperty(window, ')JS" << varName << R"JS(', {
    set: function(value) {
        window._)JS" << varName << R"JS( = value;
        __chtl.updateDOM(')JS" << varName << R"JS(', value);
    },
    get: function() {
        return window._)JS" << varName << R"JS(;
    }
});
)JS";
    }

    return runtime_script.str();
}
}

CompilationResult CompilerDispatcher::compile(
    const std::string& source,
    const std::string& source_path,
    bool default_struct,
    bool inline_css,
    bool inline_js,
    const std::string& css_output_filename,
    const std::string& js_output_filename
) {
    // 1. Scan the source to separate CHTL from other languages
    ScannedOutput scanned_output = scanner.scan(source);

    // 2. Initialize CSS and JS content strings
    std::string css_content;
    std::stringstream js_content_stream;
    std::map<std::string, std::string> placeholder_contents;

    // 3. Compile the main CHTL content first to populate managers
    Lexer lexer(scanned_output.chtl_with_placeholders);
    Parser parser(lexer, source_path);
    parser.outputHtml5Doctype = default_struct;
    auto ast = parser.parse();

    // 4. Process all fragments from the scanner
    for (const auto& pair : scanned_output.fragments) {
        const std::string& placeholder = pair.first;
        const CodeFragment& fragment = pair.second;

        switch(fragment.type) {
            case FragmentType::CSS:
                css_content += fragment.content + "\n";
                break;
            case FragmentType::JS:
                js_content_stream << fragment.content << "\n";
                placeholder_contents[placeholder] = fragment.content;
                break;
            case FragmentType::CHTL_JS: {
                std::string compiled_js = chtl_js_compiler.compile(fragment.content, parser.cjmodManager);
                js_content_stream << compiled_js << "\n";
                placeholder_contents[placeholder] = compiled_js;
                break;
            }
            case FragmentType::CHTL:
                // For CHTL fragments (e.g., from style blocks), the placeholder
                // is left in the AST to be handled by the CHTL expression evaluator.
                // The merger will just put the raw CHTL back.
                placeholder_contents[placeholder] = fragment.content;
                break;
        }
    }

    // 5. Generate and prepend runtime script if needed
    std::string runtime_script = generateRuntimeScript(parser.sharedContext);
    if (!runtime_script.empty()) {
        js_content_stream.str(runtime_script + js_content_stream.str());
    }

    // 6. Add any CSS hoisted from local style blocks by the parser
    css_content += parser.globalStyleContent;

    // 7. Generate the HTML. The generator no longer handles inlining.
    Generator generator;
    std::string html_with_placeholders = generator.generate(
        ast,
        css_content,
        js_content_stream.str(),
        parser.sharedContext,
        default_struct,
        inline_css,
        inline_js,
        css_output_filename,
        js_output_filename
    );

    // 8. Use the CodeMerger to substitute all placeholders recursively.
    std::string final_html = merger.merge(html_with_placeholders, placeholder_contents);

    // 9. If inlining, inject the final CSS and JS into the HTML.
    if (inline_css && !css_content.empty()) {
        std::string style_tag = "<style>" + css_content + "</style>";
        size_t head_pos = final_html.find("</head>");
        if (head_pos != std::string::npos) {
            final_html.insert(head_pos, style_tag);
        }
    }
    if (inline_js && !js_content_stream.str().empty()) {
        std::string script_tag = "<script>" + js_content_stream.str() + "</script>";
        size_t body_pos = final_html.find("</body>");
        if (body_pos != std::string::npos) {
            final_html.insert(body_pos, script_tag);
        }
    }

    CompilationResult result;
    result.html_content = final_html;
    result.css_content = css_content;
    result.js_content = js_content_stream.str();

    return result;
}