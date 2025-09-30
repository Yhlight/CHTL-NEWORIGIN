#include "CodeMerger.h"
#include <string>

// The merge function now runs iteratively to handle nested placeholders.
// This is necessary because a substituted fragment might itself contain another placeholder.
std::string CodeMerger::merge(const std::string& html_with_placeholders, const std::map<std::string, std::string>& placeholder_contents) {
    std::string final_output = html_with_placeholders;
    bool replaced_in_pass = true;

    // Continue merging as long as we successfully replaced a placeholder in a pass.
    while (replaced_in_pass) {
        replaced_in_pass = false;
        for (const auto& pair : placeholder_contents) {
            const std::string& placeholder = pair.first;
            const std::string& content = pair.second;

            // The placeholder format might vary (e.g., /*_CHTL_PLACEHOLDER_0_*/ or script{__JS_PLACEHOLDER_1__})
            // We must handle both the raw placeholder and cases where it's inside a block.
            std::string block_placeholder = "script {" + placeholder + "}";
            size_t pos = final_output.find(block_placeholder);
            if (pos != std::string::npos) {
                // For script blocks, we need to wrap the content in <script> tags.
                final_output.replace(pos, block_placeholder.length(), "<script>" + content + "</script>");
                replaced_in_pass = true;
                continue; // Move to the next placeholder
            }

            // Handle raw placeholders (e.g., for style properties).
            pos = final_output.find(placeholder);
            if (pos != std::string::npos) {
                final_output.replace(pos, placeholder.length(), content);
                replaced_in_pass = true;
            }
        }
    }

    return final_output;
}