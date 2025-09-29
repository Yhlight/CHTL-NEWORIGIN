#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Dispatcher/CompilerDispatcher.h"
#include "third-party/cxxopts/cxxopts.hpp"

namespace fs = std::filesystem;

// Helper function to derive output filenames from the input path.
std::string get_base_filename(const std::string& input_path) {
    fs::path path(input_path);
    return path.stem().string();
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("chtl_compiler", "A compiler for the CHTL language");

    options.add_options()
        ("h,help", "Print usage information")
        ("o,output", "Output directory for compiled files", cxxopts::value<std::string>()->default_value("."))
        ("inline", "Inline both CSS and JS into a single HTML file")
        ("inline-css", "Inline only CSS into the HTML file")
        ("inline-js", "Inline only JS into the HTML file")
        ("default-struct", "Wrap the output in a default html/head/body structure")
        ("input", "Input CHTL file", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"input"});
    options.positional_help("<input_file.chtl>");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.count("input")) {
        std::cerr << "Error: Input file is required." << std::endl;
        std::cerr << options.help() << std::endl;
        return 1;
    }

    std::string input_file = result["input"].as<std::vector<std::string>>()[0];
    fs::path output_dir = result["output"].as<std::string>();
    fs::create_directories(output_dir);

    bool inline_css = result.count("inline-css") || result.count("inline");
    bool inline_js = result.count("inline-js") || result.count("inline");
    bool default_struct = result.count("default-struct");

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
        fs::path html_filepath = output_dir / (base_filename + ".html");
        fs::path css_filepath = output_dir / (base_filename + ".css");
        fs::path js_filepath = output_dir / (base_filename + ".js");

        // TODO: Update CompilerDispatcher to accept the new options
        CompilerDispatcher dispatcher;
        CompilationResult comp_result = dispatcher.compile(
            chtlSource,
            input_file,
            (inline_css && inline_js), // Pass the old 'inline_mode' for now
            css_filepath.string(),
            js_filepath.string()
        );

        // Write the HTML file
        std::ofstream html_file(html_filepath);
        html_file << comp_result.html_content;
        html_file.close();
        std::cout << "Generated " << html_filepath << std::endl;

        // Write CSS and JS files if not inlining
        if (!inline_css && !comp_result.css_content.empty()) {
            std::ofstream css_file(css_filepath);
            css_file << comp_result.css_content;
            css_file.close();
            std::cout << "Generated " << css_filepath << std::endl;
        }
        if (!inline_js && !comp_result.js_content.empty()) {
            std::ofstream js_file(js_filepath);
            js_file << comp_result.js_content;
            js_file.close();
            std::cout << "Generated " << js_filepath << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}