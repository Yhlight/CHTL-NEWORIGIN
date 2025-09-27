#include "DependencyResolver.h"
#include "../../CHTL/CHTLLoader/Loader.h"
#include <regex>
#include <iostream>

void DependencyResolver::addFile(const std::string& path) {
    initial_files.push_back(path);
}

std::vector<std::string> DependencyResolver::resolve(const std::filesystem::path& base_directory) {
    for (const auto& file : initial_files) {
        parseDependencies(file, base_directory);
    }
    topologicalSort();
    return sorted_files;
}

void DependencyResolver::parseDependencies(const std::string& path, const std::filesystem::path& base_directory) {
    std::filesystem::path absolute_path = base_directory / path;
    std::string canonical_path_str = absolute_path.lexically_normal().string();

    if (visited_for_parsing.count(canonical_path_str)) {
        return;
    }
    visited_for_parsing.insert(canonical_path_str);
    graph[canonical_path_str] = {}; // Ensure the file is in the graph

    std::string content = Loader::loadFile(canonical_path_str);
    std::regex define_regex(R"((?:define|require)\s*\(\s*\[([^\]]*)\])");
    std::smatch match;

    if (std::regex_search(content, match, define_regex)) {
        std::string deps_str = match[1].str();
        std::regex dep_regex("'([^']*)'|\"([^\"]*)\"");
        auto words_begin = std::sregex_iterator(deps_str.begin(), deps_str.end(), dep_regex);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::string dep_path_str = (*i)[1].matched ? (*i)[1].str() : (*i)[2].str();
            if (dep_path_str.length() < 3 || dep_path_str.substr(dep_path_str.length() - 3) != ".js") {
                dep_path_str += ".js";
            }
            std::filesystem::path dep_path = absolute_path.parent_path() / dep_path_str;
            std::string canonical_dep_path = dep_path.lexically_normal().string();

            graph[canonical_path_str].push_back(canonical_dep_path);
            parseDependencies(canonical_dep_path, std::filesystem::path()); // Recurse with absolute path
        }
    }
}

void DependencyResolver::topologicalSort() {
    std::set<std::string> visited;
    for (const auto& pair : graph) {
        if (visited.find(pair.first) == visited.end()) {
            dfs(pair.first, visited, sorted_files);
        }
    }
}

void DependencyResolver::dfs(const std::string& node, std::set<std::string>& visited, std::vector<std::string>& sorted) {
    visited.insert(node);
    if (graph.count(node)) {
        for (const auto& neighbor : graph.at(node)) {
            if (visited.find(neighbor) == visited.end()) {
                dfs(neighbor, visited, sorted);
            }
        }
    }
    sorted.push_back(node);
}