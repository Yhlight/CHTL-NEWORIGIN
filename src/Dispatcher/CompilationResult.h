#pragma once

#include <string>

// A simple struct to hold the results of the compilation process.
// This allows the dispatcher to return all the necessary components to the CLI for file writing.
struct CompilationResult {
    std::string html_content;
    std::string css_content;
    std::string js_content;
};