#pragma once
#include <string>
#include <string_view>

namespace chtl {

enum class PipelineState {
    Init,
    LoadInput,
    UnifiedScan,
    Dispatch,
    CompileCHTL,
    CompileCHTLJS,
    CompileCSS,
    CompileJS,
    Merge,
    Output,
    Done,
    Error
};

struct Event {
    std::string name;
};

class StateMachine {
public:
    virtual ~StateMachine() = default;
    virtual void reset() = 0;
    virtual void onEvent(const Event& event) = 0;
};

} // namespace chtl

