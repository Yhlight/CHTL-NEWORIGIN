#include "CodeMerger.h"

namespace CHTL {

std::string CodeMerger::Merge(const CompilationResult& result) {
    std::string finalCode = result.chtlOutput.content;

    for (const auto& [placeholder_key, fragmentOutput] : result.fragmentOutputs) {
        // Here we would check the original fragment type if we had different wrapping logic
        std::string wrappedContent = "<style>" + fragmentOutput.content + "</style>";

        size_t pos = finalCode.find(placeholder_key);
        if (pos != std::string::npos) {
            finalCode.replace(pos, placeholder_key.length(), wrappedContent);
        }
    }

    return finalCode;
}

} // namespace CHTL
