#include <iostream>

#include <iostream>

#include <iostream>

// Forward declarations for the test functions
void RunLexerTests();
void RunParserTests();
void RunGeneratorTests();

int main() {
    std::cout << "Running CHTL Test Suite..." << std::endl;

    RunLexerTests();
    RunParserTests();
    RunGeneratorTests();

    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
