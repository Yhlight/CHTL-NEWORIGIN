#pragma once

#include <string>
#include <vector>
#include <memory>

// Represents a fragment of code identified by the UnifiedScanner.
struct CodeFragment {
    enum class Language {
        CHTL,
        CHTL_JS,
        CSS,
        JS,
        UNKNOWN
    };

    Language lang;
    std::string content;
};

// The UnifiedScanner is responsible for taking a raw CHTL source file
// and splitting it into fragments of different languages (CHTL, CHTL-JS, CSS, JS).
class UnifiedScanner {
public:
    explicit UnifiedScanner(const std::string& source);

    // The main entry point for the scanner.
    std::vector<std::unique_ptr<CodeFragment>> scan();

private:
    std::string source;
    size_t position;
};