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
    dynamicRenderingBindings: [],
    registerBinding: function(varName, elementId, property, unit) {
        if (!this.bindings[varName]) {
            this.bindings[varName] = [];
        }
        this.bindings[varName].push({ elementId, property, unit });
    },
    registerDynamicRendering: function(elementId, evalFunc) {
        this.dynamicRenderingBindings.push({ elementId, evalFunc });
    },
    evaluateDynamicRendering: function() {
        this.dynamicRenderingBindings.forEach(binding => {
            const element = document.getElementById(binding.elementId);
            if (element) {
                try {
                    if (binding.evalFunc()) {
                        element.style.display = '';
                    } else {
                        element.style.display = 'none';
                    }
                } catch (e) {
                    console.error("Error evaluating dynamic rendering for " + binding.elementId, e);
                    element.style.display = 'none';
                }
            }
        });
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
        this.evaluateDynamicRendering();
    }
};
document.addEventListener('DOMContentLoaded', () => __chtl.evaluateDynamicRendering());
)JS";

    // --- Handle Responsive Bindings ---
    for (const auto& pair : context.responsiveBindings) {
        const std::string& varName = pair.first;
        responsive_vars.insert(varName);
        for (const auto& binding : pair.second) {
            runtime_script << "__chtl.registerBinding('" << varName << "', '" << binding.elementId << "', '" << binding.property << "', '" << binding.unit << "');\n";
        }
    }

    // --- Handle Dynamic Rendering Bindings ---
    for (const auto& binding : context.dynamicRenderingBindings) {
        // Use std::regex_replace for a simpler and more robust substitution.
        std::string js_expr = std::regex_replace(
            binding.expression_str,
            std::regex(R"(\{\{([^}]+?)\}\})"),
            "$1"
        );
        runtime_script << "__chtl.registerDynamicRendering('" << binding.elementId << "', () => { return " << js_expr << "; });\n";
    }

    // --- Create Setters/Getters for Responsive Variables ---
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
    bool default_struct,
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
    std::stringstream loaded_scripts_stream;
    std::filesystem::path base_path = !source_path.empty() ? std::filesystem::path(source_path).parent_path() : std::filesystem::current_path();

    for (auto& pair : scanned_output.fragments) {
        const std::string& placeholder = pair.first;
        CodeFragment& fragment = pair.second;

        if (fragment.type == FragmentType::CSS) {
            css_content += fragment.content + "\n";
        } else if (fragment.type == FragmentType::CHTL_JS) {
            // First, compile the CHTL JS to get the AST
            auto chtl_js_ast = chtl_js_compiler.parse(fragment.content);

            // Now, check the AST for ScriptLoaderNodes
            for(const auto& node : chtl_js_ast) {
                if(node->getType() == CHTLJSNodeType::ScriptLoader) {
                    auto* loader_node = static_cast<ScriptLoaderNode*>(node.get());
                    DependencyResolver resolver;
                    for(const auto& path : loader_node->paths) {
                        resolver.addFile(path);
                    }
                    std::vector<std::string> resolved_files = resolver.resolve(base_path);
                    for(const auto& file_path : resolved_files) {
                        loaded_scripts_stream << Loader::loadFile(file_path) << "\n";
                    }
                }
            }

            // Generate JS from the AST
            std::string compiled_js = chtl_js_compiler.generate(chtl_js_ast);
            js_content_stream << compiled_js << "\n";
            script_fragments_for_merging[placeholder] = "<script>" + compiled_js + "</script>";

        } else if (fragment.type == FragmentType::JS) {
            js_content_stream << fragment.content << "\n";
            script_fragments_for_merging[placeholder] = "<script>" + fragment.content + "</script>";
        }
    }


    // 6. Add any CSS hoisted from local style blocks by the parser
    css_content += parser.globalStyleContent;

    // 7. Generate the HTML with a placeholder for the scripts.
    // This populates the shared context with dynamic bindings.
    const std::string js_placeholder = "__CHTL_JS_RUNTIME_PLACEHOLDER__";
    Generator generator;
    std::string html_content = generator.generate(
        ast,
        css_content,
        js_placeholder, // Pass the placeholder
        parser.sharedContext,
        parser.outputHtml5Doctype,
        inline_mode,
        default_struct,
        css_output_filename,
        js_output_filename
    );

    // 8. Generate runtime script using the now-populated context, then combine all JS.
    std::string runtime_script = generateRuntimeScript(parser.sharedContext);
    std::string final_js_content = runtime_script + loaded_scripts_stream.str() + js_content_stream.str();

    // 9. Replace the placeholder with the final combined script content, wrapped in a script tag if it's not empty.
    size_t placeholder_pos = html_content.find(js_placeholder);
    if (placeholder_pos != std::string::npos) {
        if (!final_js_content.empty() && final_js_content.find_first_not_of(" \n\r\t") != std::string::npos) {
            std::string script_tag = "<script>\n" + final_js_content + "\n</script>";
            html_content.replace(placeholder_pos, js_placeholder.length(), script_tag);
        } else {
            // If there's no script content, just remove the placeholder.
            html_content.replace(placeholder_pos, js_placeholder.length(), "");
        }
    }

    // 10. If inlining, merge the other script fragments back into the HTML.
    if (inline_mode) {
         html_content = merger.merge(html_content, script_fragments_for_merging);
    }

    CompilationResult result;
    result.html_content = html_content;
    result.css_content = css_content;
    result.js_content = final_js_content;

    return result;
}