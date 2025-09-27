#pragma once

#include <string>
#include <vector>
#include "../Scanner/UnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"
#include "CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.h"

class CompilerDispatcher {
public:
    std::string compile(const std::string& source, bool inline_mode = false);

private:
    UnifiedScanner scanner;
    CodeMerger merger;
    CHTLJSCompiler chtl_js_compiler;
};