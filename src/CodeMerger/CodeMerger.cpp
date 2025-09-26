#include "CodeMerger.h"
#include <string>

std::string CodeMerger::merge(const std::string& chtl_output, const std::map<std::string, std::string>& compiled_fragments) {
    std::string final_output = chtl_output;

    for (const auto& pair : compiled_fragments) {
        const std::string& placeholder = pair.first;
        const std::string& compiled_content = pair.second;

        // The generator will have created an empty tag like <chtl_placeholder_0></chtl_placeholder_0>
        std::string search_tag = "<" + placeholder + "></" + placeholder + ">";
        size_t pos = final_output.find(search_tag);

        if (pos != std::string::npos) {
            // Replace the placeholder tag with the actual compiled script
            std::string replacement = "<script>" + compiled_content + "</script>";
            final_output.replace(pos, search_tag.length(), replacement);
        }
    }

    return final_output;
}