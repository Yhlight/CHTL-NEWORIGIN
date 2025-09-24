#include "CodeMerger.h"

namespace codemerger {

MergeOutput CodeMerger::merge(const MergeInput& in) {
    MergeOutput out;
    // Minimal placeholder-aware concatenation; real implementation will decode
    // placeholders and reinject CSS/JS compiled outputs.
    for (const auto& s : in.chtl) out.html += s;
    for (const auto& s : in.chtljs) out.js += s;
    // Placeholders stand-in; CSS/JS will be injected later by dispatcher/backends
    for (const auto& s : in.cssPlaceholders) (void)s;
    for (const auto& s : in.jsPlaceholders) (void)s;
    return out;
}

}

