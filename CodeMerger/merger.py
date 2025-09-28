class CodeMerger:
    def merge(self, compiled_template, compiled_js_fragments):
        print("--- Merging Code ---")
        final_code = compiled_template
        for placeholder, js_code in compiled_js_fragments.items():
            final_code = final_code.replace(placeholder, js_code)
        return final_code