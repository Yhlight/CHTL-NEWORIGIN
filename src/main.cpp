#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Dispatcher/CompilerDispatcher.h"

// This main function serves as the entry point for the CHTL compiler.
// It demonstrates the new workflow: CompilerDispatcher -> Final Output.
int main() {
    std::string chtlSource = R"(
style {
    body {
        font-family: "Arial", sans-serif;
    }
}

html {
    head { }
    body {
        div {
            text: "Hello, CHTL!";
            style {
                color: blue;
            }
        }
    }
}
)";

    try {
        // 1. Dispatching Stage
        CompilerDispatcher dispatcher;
        std::string htmlOutput = dispatcher.compile(chtlSource);

        // 2. Output Results
        std::cout << "--- CHTL Source ---" << std::endl;
        std::cout << chtlSource << std::endl;
        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << htmlOutput << std::endl;

    } catch (const std::runtime_error& e) {
        // Catch and report any errors that occurred during compilation.
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}