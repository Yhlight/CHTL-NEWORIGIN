#pragma once

#include <string>
#include <vector>

// Defines the different types of code fragments the scanner can identify.
enum class FragmentType {
    CHTL,
    CSS,
    JS,
    CHTL_JS
};

// Represents a typed segment of the source code.
struct CodeFragment {
    FragmentType type;
    std::string content;
};

// The FragmentScanner class is responsible for breaking the source code
// into a sequence of typed CodeFragments. It uses a state machine
// to handle the complex, nested structure of CHTL.
class FragmentScanner {
public:
    explicit FragmentScanner(const std::string& source);
    std::vector<CodeFragment> scan();

private:
    std::string source;
    size_t position = 0;

    // The main state-driven scanning loop will go here.
    // This is a placeholder for the state machine logic.
};