#pragma once

#include <string>
#include <vector>
#include "../Scanner/UnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"
#include "CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.h"
#include "CompilationResult.h"
#include "../CHTL_JS/Util/DependencyResolver.h"
#include "../CHTL_JS/CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTL/CHTLLoader/Loader.h"

class CompilerDispatcher {
public:
    CompilationResult compile(
        const std::string& source,
        const std::string& source_path,
        bool inline_mode = false,
        bool default_struct = false,
        const std::string& css_output_filename = "style.css",
        const std::string& js_output_filename = "script.js"
    );

private:
    UnifiedScanner scanner;
    CodeMerger merger;
    CHTLJSCompiler chtl_js_compiler;
};