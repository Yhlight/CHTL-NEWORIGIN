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
    // 1. Scan the source to get a vector of typed code fragments.
    FragmentScanner scanner(source);
    std::vector<CodeFragment> fragments = scanner.scan();

    // 2. Initialize content buffers and separate CHTL from other fragments.
    std::string chtl_source;
    std::stringstream css_content_stream;
    std::stringstream js_content_stream;
    std::vector<CodeFragment> non_chtl_fragments;

    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            chtl_source += fragment.content;
        } else {
            non_chtl_fragments.push_back(fragment);
        }
    }

    // 3. Compile the main CHTL content. This builds the AST and populates
    //    the necessary managers and contexts (e.g., for templates, CJMODs).
    Lexer lexer(chtl_source);
    Parser parser(lexer, source_path);
    parser.outputHtml5Doctype = default_struct;
    auto ast = parser.parse();

    // 4. Process the non-CHTL fragments now that the parser context is available.
    for (const auto& fragment : non_chtl_fragments) {
        switch(fragment.type) {
            case FragmentType::CSS:
                css_content_stream << fragment.content << "\n";
                break;
            case FragmentType::JS:
                js_content_stream << fragment.content << "\n";
                break;
            case FragmentType::CHTL_JS: {
                // The CHTL_JS compiler needs the CJMODManager from the main parser.
                std::string compiled_js = chtl_js_compiler.compile(fragment.content, parser.cjmodManager);
                js_content_stream << compiled_js << "\n";
                break;
            }
            default: // Should not happen
                break;
        }
    }

    // 5. Generate the runtime script needed for dynamic features from the parser's context.
    std::string runtime_script = generateRuntimeScript(parser.sharedContext);
    if (!runtime_script.empty()) {
        js_content_stream.str(runtime_script + js_content_stream.str());
    }

    // 6. Add any CSS that was hoisted from local style blocks by the CHTL parser.
    css_content_stream << parser.globalStyleContent;

    // 7. Generate the final HTML. The Generator is now the single source of truth for
    //    handling inlining and linking external files. No more placeholder merging.
    Generator generator;
    std::string final_html = generator.generate(
        ast,
        css_content_stream.str(),
        js_content_stream.str(),
        parser.sharedContext,
        default_struct,
        inline_css,
        inline_js,
        css_output_filename,
        js_output_filename
    );

    // 8. Prepare and return the final compilation result.
    CompilationResult result;
    result.html_content = final_html;
    result.css_content = css_content_stream.str();
    result.js_content = js_content_stream.str();

    return result;
}