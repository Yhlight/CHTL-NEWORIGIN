#include "UnifiedScanner.h"
#include <optional>

namespace scanner {

std::vector<Segment> UnifiedScanner::scan(std::string_view source) {
    state_ = ScannerState::SeekBoundaries;
    input_ = source;
    segments_.clear();
    seekBoundaries();
    sliceAndProtect();
    strictRefine();
    emitSegments();
    state_ = ScannerState::Done;
    return segments_;
}

void UnifiedScanner::seekBoundaries() {
    // Prototype: no-op. Future: detect global style/script blocks.
}

void UnifiedScanner::sliceAndProtect() {
    const std::string_view sv = input_;
    size_t pos = 0;
    while (pos < sv.size()) {
        size_t scriptOpen = sv.find("\n    script", pos);
        size_t styleOpen = sv.find("\n        style", pos);

        size_t next = std::min(scriptOpen == std::string::npos ? sv.size() : scriptOpen,
                               styleOpen == std::string::npos ? sv.size() : styleOpen);

        if (next > pos) {
            segments_.push_back({SegmentType::CHTL, std::string(sv.substr(pos, next - pos))});
        }

        if (next == sv.size()) break;

        if (next == scriptOpen) {
            size_t blockStart = sv.find('{', scriptOpen);
            if (blockStart == std::string::npos) { pos = sv.size(); break; }
            size_t brace = 1;
            size_t i = blockStart + 1;
            while (i < sv.size() && brace > 0) {
                if (sv[i] == '{') ++brace; else if (sv[i] == '}') --brace; ++i;
            }
            segments_.push_back({SegmentType::JS_PLACEHOLDER, "JS_CODE_PLACEHOLDER_"});
            pos = i;
        } else if (next == styleOpen) {
            size_t blockStart = sv.find('{', styleOpen);
            if (blockStart == std::string::npos) { pos = sv.size(); break; }
            size_t brace = 1;
            size_t i = blockStart + 1;
            while (i < sv.size() && brace > 0) {
                if (sv[i] == '{') ++brace; else if (sv[i] == '}') --brace; ++i;
            }
            segments_.push_back({SegmentType::CSS_PLACEHOLDER, "CSS_CODE_PLACEHOLDER_"});
            pos = i;
        }
    }
}

void UnifiedScanner::strictRefine() {
    // Prototype: no-op.
}

void UnifiedScanner::emitSegments() {
    // Prototype already emitted during slicing.
}

} // namespace scanner

