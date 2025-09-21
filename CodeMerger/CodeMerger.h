#pragma once

#include "CompilerDispatcher/CompilerDispatcher.h" // For CompilationResult
#include <string>

namespace CHTL {

class CodeMerger {
public:
    std::string merge(const CompilationResult& result);
};

} // namespace CHTL
