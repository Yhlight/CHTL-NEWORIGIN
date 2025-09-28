import re

class UnifiedScanner:
    def __init__(self):
        self.js_placeholders = {}
        self.placeholder_count = 0

    def scan_script_content(self, script_content):
        # This pattern identifies CHTL JS markers. It's simple for the PoC.
        pattern = r"(\{\{.*?\}\}|->|&->)"

        modified_script = ""
        last_end = 0

        matches = list(re.finditer(pattern, script_content))

        if not matches:
            if script_content and not script_content.isspace():
                placeholder = f"_JS_CODE_PLACEHOLDER_{self.placeholder_count}_"
                self.js_placeholders[placeholder] = script_content
                self.placeholder_count += 1
                return placeholder
            return script_content

        for match in matches:
            start, end = match.span()

            js_part = script_content[last_end:start]
            if js_part:
                if not js_part.isspace():
                    placeholder = f"_JS_CODE_PLACEHOLDER_{self.placeholder_count}_"
                    self.js_placeholders[placeholder] = js_part
                    modified_script += placeholder
                    self.placeholder_count += 1
                else:
                    modified_script += js_part

            modified_script += match.group(0)
            last_end = end

        remaining_part = script_content[last_end:]
        if remaining_part:
            if not remaining_part.isspace():
                placeholder = f"_JS_CODE_PLACEHOLDER_{self.placeholder_count}_"
                self.js_placeholders[placeholder] = remaining_part
                modified_script += placeholder
                self.placeholder_count += 1
            else:
                modified_script += remaining_part

        return modified_script

    def find_script_block(self, source_code):
        try:
            script_tag_start = source_code.index("script")
            open_brace_index = source_code.index('{', script_tag_start)

            level = 1
            search_start = open_brace_index + 1
            for i in range(search_start, len(source_code)):
                if source_code[i] == '{':
                    level += 1
                elif source_code[i] == '}':
                    level -= 1
                    if level == 0:
                        close_brace_index = i
                        return open_brace_index + 1, close_brace_index
        except ValueError:
            return None, None
        return None, None

    def scan(self, source_code):
        self.js_placeholders = {}
        self.placeholder_count = 0

        content_start, content_end = self.find_script_block(source_code)

        if content_start is None:
            return source_code, {}

        script_content = source_code[content_start:content_end]
        modified_content = self.scan_script_content(script_content)

        return (
            source_code[:content_start] +
            modified_content +
            source_code[content_end:],
            self.js_placeholders
        )

if __name__ == '__main__':
    scanner = UnifiedScanner()

    # A simplified test case that the current scanner can handle
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

    modified_code, js_fragments = scanner.scan(sample_chtl)

    print("---- Modified CHTL ----")
    print(modified_code)
    print("\\n---- JS Fragments ----")
    for placeholder, code in js_fragments.items():
        print(f"{placeholder}:\\n---{code.strip()}\\n---")