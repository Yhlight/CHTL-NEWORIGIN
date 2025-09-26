#include "CodeMerger.h"
#include <string>

std::string CodeMerger::merge(const std::string& chtl_output, const std::map<std::string, std::string>& compiled_fragments) {
    std::string final_output = chtl_output;

    for (const auto& pair : compiled_fragments) {
        const std::string& placeholder = pair.first;
        const std::string& compiled_content = pair.second;

        std::string placeholder_in_script_tag = "{" + placeholder + "}";
        size_t pos = final_output.find(placeholder_in_script_tag);
        if (pos != std::string::npos) {
            final_output.replace(pos, placeholder_in_script_tag.length(), compiled_content);
        }
    }

    return final_output;
}