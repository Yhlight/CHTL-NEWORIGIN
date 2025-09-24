#pragma once
#include <string>
#include <string_view>
#include <vector>

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
    std::vector<Segment> scan(std::string_view source);

private:
    void seekBoundaries();
    void sliceAndProtect();
    void strictRefine();
    void emitSegments();

private:
    ScannerState state_ { ScannerState::SeekBoundaries };
    std::string_view input_;
    std::vector<Segment> segments_;
};

} // namespace scanner

