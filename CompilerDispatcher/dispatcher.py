import sys
import os

# Add project root to the Python path to allow for absolute imports
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Scanner.unified_scanner import UnifiedScanner
from CHTL_JS.compiler import CHTLJSCompiler
from JS.compiler import JSCompiler
from CodeMerger.merger import CodeMerger

class CompilerDispatcher:
    def __init__(self):
        self.scanner = UnifiedScanner()
        self.chtl_js_compiler = CHTLJSCompiler()
        self.js_compiler = JSCompiler()
        self.merger = CodeMerger()

    def dispatch(self, source_code):
        print("--- Dispatching ---")

        # 1. Scan the source code to separate CHTL JS and JS
        modified_template, js_fragments = self.scanner.scan(source_code)

        # 2. Compile the CHTL JS template (with placeholders)
        compiled_template = self.chtl_js_compiler.compile(modified_template)

        # 3. Compile the individual JS fragments
        compiled_js_fragments = {}
        for placeholder, js_code in js_fragments.items():
            compiled_js_fragments[placeholder] = self.js_compiler.compile(js_code)

        # 4. Merge the compiled parts
        final_code = self.merger.merge(compiled_template, compiled_js_fragments)

        return final_code

if __name__ == '__main__':
    # A simple test case to demonstrate the full pipeline
    sample_chtl = """
    div
    {
        script
        {
            let v = "Test";
            console.log("this is pure js");
            {{v}}->foo();
            let x = 10;
        }
    }
    """

    dispatcher = CompilerDispatcher()
    final_output = dispatcher.dispatch(sample_chtl)

    print("\\n---- Final Output ----")
    print(final_output)