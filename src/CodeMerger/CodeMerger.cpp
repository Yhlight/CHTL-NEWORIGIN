#include "CodeMerger.h"
#include <string>

std::string CodeMerger::merge(const std::string& chtl_output, const std::map<std::string, CodeFragment>& fragments) {
    std::string final_output = chtl_output;

    for (const auto& pair : fragments) {
        const std::string& placeholder = pair.first;
        const CodeFragment& fragment = pair.second;
        std::string final_content;

        if (fragment.type == FragmentType::CHTL_JS) {
            // For now, we just pass the CHTL JS through. In the future, this would be compiled JS.
            std::string placeholder_in_script_tag = "{" + placeholder + "}";
            size_t pos = final_output.find(placeholder_in_script_tag);
            if (pos != std::string::npos) {
                final_output.replace(pos, placeholder_in_script_tag.length(), fragment.content);
            }
        }
        // CSS fragments are handled globally by the CHTL generator for now,
        // so we don't need to re-insert them here. In the future, this might change.
    }

    return final_output;
}