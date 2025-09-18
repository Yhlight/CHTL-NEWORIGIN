#pragma once

#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <string>

namespace CHTL {

// The CompilerDispatcher orchestrates the entire compilation process.
// It takes fragments from the scanner and sends them to the appropriate compilers.
class CompilerDispatcher {
public:
    explicit CompilerDispatcher(ScannedFragments fragments);

    // Runs the compilation pipeline and returns the final HTML.
    std::string dispatch();

private:
    ScannedFragments m_fragments;
};

} // namespace CHTL
