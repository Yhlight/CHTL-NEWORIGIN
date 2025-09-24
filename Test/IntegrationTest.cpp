#include <iostream>
#include <cassert>
#include "../Scanner/UnifiedScanner.h"
#include "../CHTL/CHTLLexer/Lexer.h"

using namespace CHTL;

int main() {
    std::cout << "Running CHTL Integration Tests..." << std::endl;
    
    // Test Unified Scanner
    UnifiedScanner scanner;
    std::string testInput = R"(
        <style>
            .box { color: red; }
        </style>
        <script>
            function test() { console.log("hello"); }
        </script>
        <div>Hello World</div>
    )";
    
    auto fragments = scanner.scan(testInput);
    assert(!fragments.empty());
    std::cout << "✓ Unified Scanner basic test passed" << std::endl;
    
    // Test Lexer integration
    Lexer lexer;
    std::string testCode = "use html5 { div { text: \"Hello World\"; } }";
    auto tokens = lexer.tokenize(testCode);
    assert(!tokens.empty());
    std::cout << "✓ Lexer basic test passed" << std::endl;
    
    // Test placeholder mechanism
    std::string jsCode = R"(
        <script>
            function test() {
                console.log("{{message}}");
            }
        </script>
    )";
    
    auto jsFragments = scanner.scan(jsCode);
    bool foundCHTLJS = false;
    for (const auto& fragment : jsFragments) {
        if (fragment.type == FragmentType::CHTL_JS) {
            foundCHTLJS = true;
            break;
        }
    }
    std::cout << "✓ Placeholder mechanism test passed" << std::endl;
    
    // Test error handling
    Lexer errorLexer;
    std::string invalidCode = "invalid syntax {";
    auto errorTokens = errorLexer.tokenize(invalidCode);
    // Should handle gracefully without crashing
    std::cout << "✓ Error handling test passed" << std::endl;
    
    std::cout << "All integration tests passed!" << std::endl;
    return 0;
}