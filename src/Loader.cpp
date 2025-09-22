#include "Loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

// A simplified helper to read a file.
std::string read_file_content(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for import " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Loader::get_base_path(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return filepath.substr(0, pos + 1);
    }
    return "";
}

std::string Loader::load(const std::string& main_filepath) {
    std::set<std::string> loaded_files;
    return load_and_preprocess(main_filepath, loaded_files);
}

// This is a naive implementation. It finds imports and pastes the file content
// in place of the import statement. A more robust solution would build a
// dependency graph.
std::string Loader::load_and_preprocess(const std::string& filepath, std::set<std::string>& loaded_files) {
    if (loaded_files.count(filepath)) {
        return ""; // Prevent circular dependencies
    }
    loaded_files.insert(filepath);

    std::string content = read_file_content(filepath);
    if (content.empty()) {
        return "";
    }

    // Regex to find [Import] @Chtl from "path";
    // This is a simplification and might be brittle.
    std::regex import_regex(R"(\[Import\]\s*@Chtl\s*from\s*["']?([^"';]+)["']?\s*;)");
    std::smatch match;

    std::string base_path = get_base_path(filepath);
    auto it = content.cbegin();
    std::string processed_content;

    while (std::regex_search(it, content.cend(), match, import_regex)) {
        processed_content += match.prefix();
        std::string import_path = base_path + match[1].str();
        processed_content += load_and_preprocess(import_path, loaded_files);
        it = match.suffix().first;
    }
    processed_content += std::string(it, content.cend());

    return processed_content;
}
