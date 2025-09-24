#include "CodeMerger.hpp"

namespace chtl::merger {

MergeOutput CodeMerger::merge(const std::string& chtlOut,
                              const std::string& cssOut,
                              const std::string& jsOut,
                              bool inlineHtml,
                              bool defaultStruct) const {
    MergeOutput out;
    if (inlineHtml) {
        out.html = std::string("<html>") + (defaultStruct ? "<head>" : "") +
                   (cssOut.empty() ? "" : std::string("<style>") + cssOut + "</style>") +
                   (defaultStruct ? "</head><body>" : "") +
                   chtlOut +
                   (jsOut.empty() ? "" : std::string("<script>") + jsOut + "</script>") +
                   (defaultStruct ? "</body>" : "") + "</html>";
    } else {
        out.html = chtlOut;
        out.css = cssOut;
        out.js = jsOut;
    }
    return out;
}

}
