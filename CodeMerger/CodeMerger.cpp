#include "CodeMerger.h"

namespace CHTL {

std::string CodeMerger::merge(
    std::string html_with_placeholders,
    const std::map<std::string, std::string>& compiled_scripts
) {
    std::string final_html = html_with_placeholders;

    for (const auto& pair : compiled_scripts) {
        const std::string& placeholder = pair.first;
        const std::string& js_code = pair.second;

        // Construct the full script tag
        std::string script_tag = "<script>" + js_code + "</script>";

        // Find and replace the placeholder
        size_t pos = final_html.find(placeholder);
        if (pos != std::string::npos) {
            final_html.replace(pos, placeholder.length(), script_tag);
        }
    }

    return final_html;
}

} // namespace CHTL
