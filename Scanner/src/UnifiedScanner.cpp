#include "UnifiedScanner.hpp"

namespace chtl::scanner {

std::vector<Fragment> UnifiedScanner::scan(const std::string& source) const {
    // Minimal conservative split: extract top-level style and script blocks;
    // mark script as JsPlaceholder, global style as CssPlaceholder.
    std::vector<Fragment> out;
    size_t pos = 0;
    while (pos < source.size()) {
        size_t stylePos = source.find("\nstyle\n{", pos);
        size_t scriptPos = source.find("\nscript\n{", pos);
        size_t next = std::min(stylePos == std::string::npos ? source.size() : stylePos,
                               scriptPos == std::string::npos ? source.size() : scriptPos);
        if (next > pos) {
            out.push_back({ FragmentType::Chtl, source.substr(pos, next - pos) });
        }
        if (next == stylePos) {
            size_t end = source.find("}\n", stylePos + 1);
            std::string block = end == std::string::npos ? source.substr(stylePos) : source.substr(stylePos, end - stylePos + 2);
            out.push_back({ FragmentType::CssPlaceholder, block });
            pos = end == std::string::npos ? source.size() : end + 2;
        } else if (next == scriptPos) {
            size_t end = source.find("}\n", scriptPos + 1);
            std::string block = end == std::string::npos ? source.substr(scriptPos) : source.substr(scriptPos, end - scriptPos + 2);
            out.push_back({ FragmentType::JsPlaceholder, block });
            pos = end == std::string::npos ? source.size() : end + 2;
        } else {
            pos = source.size();
        }
    }
    return out;
}

}
