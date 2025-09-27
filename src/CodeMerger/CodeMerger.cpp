#include "CodeMerger.h"
#include <string>

std::string CodeMerger::merge(const std::string& chtl_output, const std::map<std::string, std::string>& compiled_fragments) {
    std::string final_output = chtl_output;

    for (const auto& pair : compiled_fragments) {
        const std::string& placeholder = pair.first;
        const std::string& compiled_content = pair.second;

        // The placeholder now includes the braces, e.g., "chtl_placeholder_0{}".
        std::string search_tag = placeholder + "{}";
        size_t pos = final_output.find(search_tag);

        if (pos != std::string::npos) {
            // The dispatcher is now responsible for wrapping the content in appropriate tags.
            // The merger's only job is to replace the placeholder.
            final_output.replace(pos, search_tag.length(), compiled_content);
        }
    }

    return final_output;
}