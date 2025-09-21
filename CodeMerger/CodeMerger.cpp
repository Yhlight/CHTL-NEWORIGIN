#include "CodeMerger.h"

namespace CHTL {

std::string CodeMerger::merge(const CompilationResult& result) {
    std::string final_html = result.html_output;

    for (const auto& pair : result.css_outputs) {
        const std::string& placeholder = pair.first;
        const std::string& css_content = pair.second;

        std::string style_tag = "<style>\n" + css_content + "\n</style>";

        size_t pos = final_html.find(placeholder);
        if (pos != std::string::npos) {
            final_html.replace(pos, placeholder.length(), style_tag);
        }
    }

    return final_html;
}

} // namespace CHTL
