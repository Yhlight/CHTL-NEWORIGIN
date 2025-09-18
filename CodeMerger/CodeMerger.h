#pragma once

#include <string>
#include <map>

namespace CHTL {

// The CodeMerger is responsible for the final step of compilation:
// merging the generated JavaScript back into the HTML by replacing placeholders.
class CodeMerger {
public:
    std::string merge(
        std::string html_with_placeholders,
        const std::map<std::string, std::string>& compiled_scripts
    );
};

} // namespace CHTL
