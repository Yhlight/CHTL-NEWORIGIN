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

    // 3. Compile the main CHTL content first to populate managers
    Lexer lexer(scanned_output.chtl_with_placeholders);
    Parser parser(lexer, source_path);
    auto ast = parser.parse();

    // 4. Post-process script fragments to resolve named origin blocks
    for (auto& pair : scanned_output.fragments) {
        CodeFragment& fragment = pair.second;
        if (fragment.type == FragmentType::JS || fragment.type == FragmentType::CHTL_JS) {
            std::string current_content = fragment.content;
            std::string final_content;
            std::regex origin_regex(R"(\[Origin\]\s*@JavaScript\s+([a-zA-Z0-9_]+)\s*;)");
            auto search_start = current_content.cbegin();
            std::smatch match;

            while (std::regex_search(search_start, current_content.cend(), match, origin_regex)) {
                final_content += match.prefix();
                std::string origin_name = match[1];

                OriginNode* origin_node = parser.templateManager.getNamedOrigin("_global", origin_name);

                if (origin_node) {
                    final_content += origin_node->content;
                } else {
                    throw std::runtime_error("Named origin block '" + origin_name + "' used in script fragment but not found.");
                }
                search_start = match.suffix().first;
            }
            final_content += std::string(search_start, current_content.cend());
            fragment.content = final_content;
        }
    }

    // 5. Process all fragments from the scanner
    for (const auto& pair : scanned_output.fragments) {
        const std::string& placeholder = pair.first;
        const CodeFragment& fragment = pair.second;

        if (fragment.type == FragmentType::CSS) {
            css_content += fragment.content + "\n";
        } else if (fragment.type == FragmentType::CHTL_JS) {
            // std::string compiled_js = chtl_js_compiler.compile(fragment.content);
            // js_content_stream << compiled_js << "\n";
            // script_fragments_for_merging[placeholder] = "<script>" + compiled_js + "</script>";
            // For now, treat CHTL_JS as plain JS to avoid linker errors.
            js_content_stream << fragment.content << "\n";
            script_fragments_for_merging[placeholder] = "<script>" + fragment.content + "</script>";
        } else if (fragment.type == FragmentType::JS) {
            js_content_stream << fragment.content << "\n";
            script_fragments_for_merging[placeholder] = "<script>" + fragment.content + "</script>";
        }
    }


    // 6. Generate and prepend runtime script if needed
    std::string runtime_script = generateRuntimeScript(parser.sharedContext);
    if (!runtime_script.empty()) {
        js_content_stream.str(runtime_script + js_content_stream.str());
    }

    // 7. Add any CSS hoisted from local style blocks by the parser
    css_content += parser.globalStyleContent;

    // 8. Generate the HTML.
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

    // 9. If inlining, merge the script placeholders back into the HTML.
    if (inline_mode) {
         html_content = merger.merge(html_content, script_fragments_for_merging);
    }

    CompilationResult result;
    result.html_content = html_content;
    result.css_content = css_content;
    result.js_content = js_content_stream.str();

    return result;
}