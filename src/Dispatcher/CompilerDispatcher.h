#pragma once

#include <string>
#include <vector>
#include "../Scanner/UnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"
#include "CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.h"
#include "CompilationResult.h"

class CompilerDispatcher {
public:
    CompilationResult compile(
        const std::string& source,
        bool inline_mode = false,
        const std::string& css_output_filename = "style.css",
        const std::string& js_output_filename = "script.js"
    );

private:
    UnifiedScanner scanner;
    CodeMerger merger;
    CHTLJSCompiler chtl_js_compiler;
};