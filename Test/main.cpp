#include <iostream>
#include <string>
#include <algorithm>
#include <regex>
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"

// Function to normalize whitespace for robust comparison.
std::string normalize_whitespace(std::string str) {
    // 1. Replace all types of whitespace characters with a standard space.
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::isspace(c) ? ' ' : c;
    });

    // 2. Collapse multiple spaces into a single space.
    std::regex re(" +");
    str = std::regex_replace(str, re, " ");

    // 3. Trim leading and trailing spaces.
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return ""; // The string is all spaces
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}


// Helper function to compare strings and report differences
void check_output(const std::string& actual_raw, const std::string& expected_raw) {
    std::string actual = normalize_whitespace(actual_raw);
    std::string expected = normalize_whitespace(expected_raw);

    if (actual == expected) {
        std::cout << "[SUCCESS] Output matches expected result.\n";
    } else {
        std::cerr << "[FAILURE] Output does not match expected result.\n";
        std::cerr << "--- Expected (Normalized) ---\n" << expected << "\n";
        std::cerr << "--- Actual (Normalized) ---\n" << actual << "\n";
        exit(1);
    }
}

int main() {
    std::string input = R"~(
// Comprehensive test for CHTL
html {
    head {
        title {
            text { "Test & Report" }
        }
        style {
            body {
                font-family: Arial, sans-serif;
                background-color: #f0f0f0;
            }
            .container {
                width: 80%;
            }
        }
    }
    body {
        div {
            id: "main-content";
            class: container;
            z-index: 10;
        }
        p {
            text { "This is a <span> tag." }
        }
    }
}
)~";

    std::cout << "--- Input CHTL ---\n" << input << "\n\n";

    Lexer l(input);
    Parser p(l);
    auto program = p.ParseProgram();

    auto errors = p.Errors();
    if (!errors.empty()) {
        std::cerr << "--- Parser Errors ---\n";
        for (const auto& msg : errors) {
            std::cerr << msg << "\n";
        }
        return 1;
    }

    Generator g;
    std::string html_output = g.Generate(program.get());

    std::cout << "--- Generated HTML ---\n" << html_output << "\n\n";

    std::string expected_output = "<html><head><title>Test &amp; Report</title><style> body { font-family: Arial, sans-serif; background-color: #f0f0f0; } .container { width: 80%; } </style></head><body><div id=\"main-content\" class=\"container\" z-index=\"10\"></div><p>This is a &lt;span&gt; tag.</p></body></html>";

    check_output(html_output, expected_output);

    return 0;
}
