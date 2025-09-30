#pragma once

#include <string>
#include <vector>
#include "../FragmentScanner/FragmentScanner.h"
#include "CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.h"
#include "CompilationResult.h"

class CompilerDispatcher {
public:
    CompilationResult compile(
        const std::string& source,
        const std::string& source_path,
        bool default_struct,
        bool inline_css,
        bool inline_js,
        const std::string& css_output_filename,
        const std::string& js_output_filename
    );

private:
    // The CHTL JS Compiler is kept as it processes fragments.
    CHTLJSCompiler chtl_js_compiler;
};