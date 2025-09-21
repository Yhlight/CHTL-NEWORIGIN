#include "UnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

// Helper to manage placeholder creation
std::string create_placeholder(const std::string& type, int& count) {
    return "__" + type + "_PLACEHOLDER_" + std::to_string(count++) + "__";
}

ScanResult UnifiedScanner::scan(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    ScanResult result;
    std::string main_source;
    int brace_level = 0;
    int placeholder_css_count = 0;
    int placeholder_js_count = 0;
    int placeholder_chtl_js_count = 0;

    for (size_t i = 0; i < source.length(); ++i) {
        // Naive check for global style/script blocks.
        // A more robust implementation would use the CHTL parser itself
        // to build a simple CST to find these blocks accurately.
        // For now, we assume they are at a low brace level.
        bool is_potential_block = (brace_level <= 2);

        if (is_potential_block && i + 5 < source.length() && source.substr(i, 5) == "style") {
            size_t opening_brace = source.find('{', i);
            if (opening_brace != std::string::npos) {
                // ... Logic to extract global style block ...
                // This is complex, will defer for now to focus on scripts
            }
        }

        if (is_potential_block && i + 6 < source.length() && source.substr(i, 6) == "script") {
            size_t opening_brace = source.find('{', i);
            if (opening_brace != std::string::npos) {
                main_source += source.substr(i, opening_brace + 1 - i);
                i = opening_brace + 1;

                std::string script_content;
                int script_brace_level = 1;
                size_t script_start = i;

                while(i < source.length() && script_brace_level > 0) {
                    if (source[i] == '{') script_brace_level++;
                    else if (source[i] == '}') script_brace_level--;
                    i++;
                }

                script_content = source.substr(script_start, i - script_start - 1);

                // Now, sub-scan the script content
                std::string script_with_placeholders;
                size_t last_pos = 0;
                while(last_pos < script_content.length()) {
                    size_t cjs_start = script_content.find("{{", last_pos);
                    if (cjs_start == std::string::npos) {
                        // No more CHTL JS, the rest is plain JS
                        if(last_pos < script_content.length()) {
                            std::string js_frag = script_content.substr(last_pos);
                            std::string ph = create_placeholder("JS", placeholder_js_count);
                            result.fragments[ph] = {FragmentType::JS_PLAIN, js_frag};
                            script_with_placeholders += ph;
                        }
                        break;
                    }

                    // Handle the plain JS before the CHTL JS
                    if (cjs_start > last_pos) {
                        std::string js_frag = script_content.substr(last_pos, cjs_start - last_pos);
                        std::string ph = create_placeholder("JS", placeholder_js_count);
                        result.fragments[ph] = {FragmentType::JS_PLAIN, js_frag};
                        script_with_placeholders += ph;
                    }

                    // Handle the CHTL JS
                    size_t cjs_end = script_content.find("}}", cjs_start);
                    if (cjs_end == std::string::npos) { /* error handling */ break; }
                    cjs_end += 2; // include the "}}"

                    std::string cjs_frag = script_content.substr(cjs_start, cjs_end - cjs_start);
                    std::string ph = create_placeholder("CHTLJS", placeholder_chtl_js_count);
                    result.fragments[ph] = {FragmentType::CHTL_JS, cjs_frag};
                    script_with_placeholders += ph;

                    last_pos = cjs_end;
                }
                main_source += script_with_placeholders;
                main_source += "}";
                i--; // Decrement i because the outer loop will increment it.
                continue;
            }
        }

        main_source += source[i];
        if (source[i] == '{') brace_level++;
        else if (source[i] == '}') brace_level--;
    }

    result.source_with_placeholders = main_source;
    return result;
}

} // namespace CHTL
