#include <iostream>
#include <cassert>
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLLexer/Lexer.h"

using namespace CHTL;

int main() {
    std::cout << "Running CHTL Parser Tests..." << std::endl;
    
    // Test basic CHTL parsing
    CHTLParser parser;
    parser.setDebugMode(true);
    
    std::string testCode = R"(
        use html5 {
            div {
                text: "Hello World";
                style: "color: red;";
            }
        }
    )";
    
    std::cout << "Testing basic CHTL parsing..." << std::endl;
    auto result = parser.parse(testCode);
    
    if (result) {
        std::cout << "✓ Basic CHTL parsing test passed" << std::endl;
        std::cout << "  Parsed elements: " << parser.getParsedElementCount() << std::endl;
        std::cout << "  Parsed templates: " << parser.getParsedTemplateCount() << std::endl;
        std::cout << "  Parsed customs: " << parser.getParsedCustomCount() << std::endl;
    } else {
        std::cout << "✗ Basic CHTL parsing test failed" << std::endl;
        std::cout << "Errors: " << parser.getErrorSummary() << std::endl;
        return 1;
    }
    
    // Test use statement parsing
    std::string useCode = "use html5 { }";
    auto useResult = parser.parse(useCode);
    
    if (useResult) {
        std::cout << "✓ Use statement parsing test passed" << std::endl;
    } else {
        std::cout << "✗ Use statement parsing test failed" << std::endl;
        std::cout << "Errors: " << parser.getErrorSummary() << std::endl;
        return 1;
    }
    
    // Test element parsing
    std::string elementCode = R"(
        use html5 {
            div {
                text: "Test";
            }
            span {
                text: "Another test";
            }
        }
    )";
    
    auto elementResult = parser.parse(elementCode);
    
    if (elementResult) {
        std::cout << "✓ Element parsing test passed" << std::endl;
        std::cout << "  Parsed elements: " << parser.getParsedElementCount() << std::endl;
    } else {
        std::cout << "✗ Element parsing test failed" << std::endl;
        std::cout << "Errors: " << parser.getErrorSummary() << std::endl;
        return 1;
    }
    
    // Test error handling
    std::string invalidCode = "invalid syntax";
    parser.clearErrors();
    auto invalidResult = parser.parse(invalidCode);
    
    if (!invalidResult && parser.hasError()) {
        std::cout << "✓ Error handling test passed" << std::endl;
        std::cout << "  Error summary: " << parser.getErrorSummary() << std::endl;
    } else {
        std::cout << "✗ Error handling test failed" << std::endl;
        return 1;
    }
    
    // Test ParserUtils
    assert(ParserUtils::isValidElementName("div"));
    assert(ParserUtils::isValidElementName("my-element"));
    assert(!ParserUtils::isValidElementName("123invalid"));
    assert(!ParserUtils::isValidElementName(""));
    std::cout << "✓ ParserUtils validation test passed" << std::endl;
    
    // Test ParserFactory
    auto factoryParser = ParserFactory::createParser();
    auto strictParser = ParserFactory::createStrictParser();
    auto debugParser = ParserFactory::createDebugParser();
    
    if (factoryParser && strictParser && debugParser) {
        std::cout << "✓ ParserFactory test passed" << std::endl;
    } else {
        std::cout << "✗ ParserFactory test failed" << std::endl;
        return 1;
    }
    
    std::cout << "All CHTL Parser tests passed!" << std::endl;
    return 0;
}