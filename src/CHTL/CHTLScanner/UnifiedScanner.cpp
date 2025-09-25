#include "UnifiedScanner.h"

UnifiedScanner::UnifiedScanner(const std::string& source)
    : source(source), position(0) {}

std::vector<std::unique_ptr<CodeFragment>> UnifiedScanner::scan() {
    // For now, we'll treat the entire source as a single CHTL fragment.
    // This will be expanded later to handle CHTL JS.
    auto fragment = std::make_unique<CodeFragment>();
    fragment->lang = CodeFragment::Language::CHTL;
    fragment->content = source;

    std::vector<std::unique_ptr<CodeFragment>> fragments;
    fragments.push_back(std::move(fragment));

    return fragments;
}