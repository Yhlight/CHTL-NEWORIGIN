#include <string>
#include <vector>
#include <string_view>
#include <optional>

namespace scanner {

enum class SegmentType {
    CHTL,
    CHTL_JS,
    CSS_PLACEHOLDER,
    JS_PLACEHOLDER
};

struct Segment {
    SegmentType type;
    std::string content;
};

enum class ScannerState {
    SeekBoundaries,
    SliceAndProtect,
    StrictRefine,
    EmitSegments,
    Done
};

class UnifiedScanner {
public:
    UnifiedScanner() = default;

    std::vector<Segment> scan(std::string_view source) {
        state_ = ScannerState::SeekBoundaries;
        input_ = source;
        segments_.clear();

        // Minimal placeholder prototype per CHTL.md (strict: no private extensions)
        // Strategy hooks to be injected later via interfaces.
        seekBoundaries();
        sliceAndProtect();
        strictRefine();
        emitSegments();
        state_ = ScannerState::Done;
        return segments_;
    }

private:
    void seekBoundaries() {
        // Prototype: no-op. Future: detect global style/script blocks.
    }

    void sliceAndProtect() {
        // Prototype: treat entire input as CHTL, replace <script> body with JS placeholder,
        // and global style with CSS placeholder where necessary (simple heuristic).
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
                // naive capture until closing '}' of this block
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

    void strictRefine() {
        // Prototype: no-op. Future: refine CHTL vs CHTL_JS inside placeholders with {{ }}.
    }

    void emitSegments() {
        // Prototype already emitted during slicing.
    }

private:
    ScannerState state_ { ScannerState::SeekBoundaries };
    std::string_view input_;
    std::vector<Segment> segments_;
};

} // namespace scanner

