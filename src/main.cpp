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

// This main function serves as the entry point for the CHTL compiler.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [--inline] [--default-struct]" << std::endl;
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    bool inline_mode = false;
    bool default_struct_mode = false;
    std::string input_file;

    auto inline_it = std::find(args.begin(), args.end(), "--inline");
    if (inline_it != args.end()) {
        inline_mode = true;
        args.erase(inline_it);
    }

    auto struct_it = std::find(args.begin(), args.end(), "--default-struct");
    if (struct_it != args.end()) {
        default_struct_mode = true;
        args.erase(struct_it);
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
        fs::path input_path_obj(input_file);
        fs::path output_dir = input_path_obj.has_parent_path() ? input_path_obj.parent_path() : fs::current_path();
        std::string base_filename = input_path_obj.stem().string();

        fs::path html_filepath = output_dir / (base_filename + ".html");
        fs::path css_filepath = output_dir / (base_filename + ".css");
        fs::path js_filepath = output_dir / (base_filename + ".js");

        CompilerDispatcher dispatcher;
        CompilationResult result = dispatcher.compile(
            chtlSource,
            input_file,
            inline_mode,
            default_struct_mode,
            css_filepath.filename().string(),
            js_filepath.filename().string()
        );

        // Write the HTML file
        std::ofstream html_file(html_filepath);
        html_file << result.html_content;
        html_file.close();
        std::cout << "Generated " << html_filepath.string() << std::endl;

        // Write CSS and JS files if not inlining
        if (!inline_mode) {
            if (!result.css_content.empty()) {
                std::ofstream css_file(css_filepath);
                css_file << result.css_content;
                css_file.close();
                std::cout << "Generated " << css_filepath.string() << std::endl;
            }
            if (!result.js_content.empty()) {
                std::ofstream js_file(js_filepath);
                js_file << result.js_content;
                js_file.close();
                std::cout << "Generated " << js_filepath.string() << std::endl;
            }
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}