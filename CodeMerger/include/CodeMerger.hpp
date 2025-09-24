#pragma once

#include <string>

namespace chtl::merger {

struct MergeOutput {
    std::string html;
    std::string css;
    std::string js;
};

class CodeMerger {
public:
    MergeOutput merge(const std::string& chtlOut,
                      const std::string& cssOut,
                      const std::string& jsOut,
                      bool inlineHtml,
                      bool defaultStruct) const;
};

}
