#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Dispatcher/CompilerDispatcher.h"
#include <algorithm>

namespace fs = std::filesystem;

// Helper function to derive output filenames from the input path.
std::string get_base_filename(const std::string& input_path) {
    fs::path path(input_path);
    return path.stem().string();
}

// This main function serves as the entry point for the CHTL compiler.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [--inline]" << std::endl;
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    bool inline_mode = false;
    std::string input_file;

    auto it = std::find(args.begin(), args.end(), "--inline");
    if (it != args.end()) {
        inline_mode = true;
        args.erase(it);
    }

    if (args.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        return 1;
    }
    input_file = args[0];

    std::ifstream file_stream(input_file);
    if (!file_stream) {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    std::string chtlSource = buffer.str();

    try {
        std::string base_filename = get_base_filename(input_file);
        std::string html_filename = base_filename + ".html";
        std::string css_filename = base_filename + ".css";
        std::string js_filename = base_filename + ".js";

        CompilerDispatcher dispatcher;
        CompilationResult result = dispatcher.compile(
            chtlSource,
            input_file,
            inline_mode,
            css_filename,
            js_filename
        );

        // Write the HTML file
        std::ofstream html_file(html_filename);
        html_file << result.html_content;
        html_file.close();
        std::cout << "Generated " << html_filename << std::endl;

        // Write CSS and JS files if not inlining
        if (!inline_mode) {
            if (!result.css_content.empty()) {
                std::ofstream css_file(css_filename);
                css_file << result.css_content;
                css_file.close();
                std::cout << "Generated " << css_filename << std::endl;
            }
            if (!result.js_content.empty()) {
                std::ofstream js_file(js_filename);
                js_file << result.js_content;
                js_file.close();
                std::cout << "Generated " << js_filename << std::endl;
            }
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}