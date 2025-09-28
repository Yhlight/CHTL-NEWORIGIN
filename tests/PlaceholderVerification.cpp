#include <cassert>
#include <string>
#include <vector>
#include <memory>

#include "CHTL_JS/CHTLLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSNode.h"
#include "CHTL_JS/CHTLJSNode/ListenNode.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "CHTL_JS/CHTLJSManage/VirtualObjectManager.h"
#include "Dispatcher/CompilerDispatcher.h"
#include "Dispatcher/CompilationResult.h"

// Helper function to remove all whitespace from a string
extern std::string remove_whitespace(std::string str);

void test_placeholder_based_parsing_and_generation() {
    // This source code simulates the output of the "Unified Scanner".
    // The raw JS `() => { console.log("clicked"); }` has been replaced
    // with a reserved placeholder.
    std::string source_with_placeholder = R"(
        script {
            {{.my-button}}->Listen {
                click: __CHTL_RESERVED_PLACEHOLDER__{uuid-1234}__
            }
        }
    )";

    // The CompilerDispatcher will internally invoke the CHTLJSCompiler,
    // which uses the refactored Lexer, Parser, and Generator.
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source_with_placeholder, "test.chtl", true);

    // The expected output should be the JavaScript code with the placeholder
    // correctly passed through by the CHTLJSGenerator. The final code merger
    // would then replace the placeholder with the actual code.
    std::string expected_js = R"(
        document.querySelector('.my-button').addEventListener('click', __CHTL_RESERVED_PLACEHOLDER__{uuid-1234}__);
    )";

    // We compare the generated JS content with our expectation.
    assert(remove_whitespace(result.js_content) == remove_whitespace(expected_js));
}