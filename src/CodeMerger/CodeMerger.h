#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include "CompilerDispatcher/CompilerDispatcher.h"
#include <string>

namespace CHTL {

class CodeMerger {
public:
    std::string Merge(const CompilationResult& result);
};

} // namespace CHTL

#endif // CODE_MERGER_H
