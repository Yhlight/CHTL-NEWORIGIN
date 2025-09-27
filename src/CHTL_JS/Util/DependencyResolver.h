#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>

class DependencyResolver {
public:
    void addFile(const std::string& path);
    std::vector<std::string> resolve(const std::filesystem::path& base_directory);

private:
    void parseDependencies(const std::string& path, const std::filesystem::path& base_directory);
    void topologicalSort();
    void dfs(const std::string& node, std::set<std::string>& visited, std::vector<std::string>& sorted);

    std::vector<std::string> initial_files;
    std::map<std::string, std::vector<std::string>> graph;
    std::vector<std::string> sorted_files;
    std::set<std::string> visited_for_parsing;
};