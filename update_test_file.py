import os

file_path = "test.chtl"
content = r"""
# CHTL document with style expressions

html {
    head {
        title { text { "CHTL Style Expressions Test" } }
    }

    body {
        div {
            class: "test-box";
            style {
                width: 100 + 50;
                height: 300 / 2;
                font-size: 10 > 5 ? 16 : 12;
                color: "red";
                padding: 10 px;
            }
            text { "This box is styled with CHTL expressions!" }
        }
    }
}
"""

with open(file_path, "w") as f:
    f.write(content.strip())

print(f"File '{file_path}' updated successfully.")
