class CHTLJSCompiler:
    def compile(self, chtl_js_code):
        # For the PoC, this will be a simple transformation.
        # It will simulate compiling CHTL JS to standard JS.
        print(f"--- Compiling CHTL JS ---\\n{chtl_js_code}\\n-------------------------")
        # A simple replacement for demonstration purposes
        return chtl_js_code.replace("->", ".").replace("{{", "document.querySelector('").replace("}}", "')")