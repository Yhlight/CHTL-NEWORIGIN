#include <iostream>
#include <string>
#include <vector>
#include "Scanner/UnifiedScanner.h"

using namespace CHTL::Scanner;

int main() {
    std::string input = "html\n{\n    head\n    {\n        title\n        {\n            \"Simple Test\"\n        }\n    }\n    \n    body\n    {\n        div\n        {\n            text\n            {\n                \"Hello World\"\n            }\n        }\n    }\n}";
    
    std::cout << "DEBUG: main - input: '" << input << "'" << std::endl;
    
    UnifiedScanner scanner;
    std::cout << "DEBUG: main - scanner created" << std::endl;
    
    std::cout << "DEBUG: main - calling scanner.scan" << std::endl;
    std::cout.flush();
    ScanResult result = scanner.scan(input);
    std::cout << "DEBUG: main - scan completed, success: " << result.success << std::endl;
    
    if (!result.success) {
        std::cerr << "Error: " << result.errorMessage << std::endl;
        return 1;
    }
    
    std::cout << "Scan completed successfully" << std::endl;
    std::cout << "Fragments found: " << result.fragments.size() << std::endl;
    
    for (size_t i = 0; i < result.fragments.size(); ++i) {
        std::cout << "Fragment " << i << ": " << static_cast<int>(result.fragments[i].type) << " '" << result.fragments[i].content << "'" << std::endl;
    }
    
    return 0;
}