#include "CodeMerger.h"

std::string CodeMerger::merge(std::string compiled_source, const std::map<std::string, std::string>& placeholder_map) {
    for (const auto& pair : placeholder_map) {
        const std::string& placeholder = pair.first;
        const std::string& original_code = pair.second;

        size_t pos = compiled_source.find(placeholder);
        while (pos != std::string::npos) {
            compiled_source.replace(pos, placeholder.length(), original_code);
            pos = compiled_source.find(placeholder, pos + original_code.length());
        }
    }
    return compiled_source;
}