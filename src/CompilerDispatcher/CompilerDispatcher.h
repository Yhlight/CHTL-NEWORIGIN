#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "Scanner/UnifiedScanner.h"
#include <string>
#include <map>

namespace CHTL {

// This will hold the output from one of the compilers
struct CompilationOutput {
    std::string content;
    // We can add more fields later, like source maps, etc.
};

// This will hold the results from all compilers, ready for merging
struct CompilationResult {
    CompilationOutput chtlOutput;
    std::map<std::string, CompilationOutput> fragmentOutputs; // placeholder -> output
};


class CompilerDispatcher {
public:
    CompilationResult Dispatch(const ScanResult& scanResult);
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H
