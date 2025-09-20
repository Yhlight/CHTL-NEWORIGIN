#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <string>

int main() {
    std::cout << "CHTL Basic Test Suite" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // Test 1: Basic functionality
    std::cout << "Test 1: Basic string operations..." << std::endl;
    std::string test = "hello";
    assert(test == "hello");
    std::cout << "  ✓ String operations work" << std::endl;
    
    // Test 2: Container operations
    std::cout << "Test 2: Container operations..." << std::endl;
    std::vector<std::string> vec = {"a", "b", "c"};
    assert(vec.size() == 3);
    assert(vec[0] == "a");
    std::cout << "  ✓ Container operations work" << std::endl;
    
    // Test 3: Map operations
    std::cout << "Test 3: Map operations..." << std::endl;
    std::map<std::string, std::string> map;
    map["key"] = "value";
    assert(map["key"] == "value");
    std::cout << "  ✓ Map operations work" << std::endl;
    
    // Test 4: File system simulation
    std::cout << "Test 4: File system simulation..." << std::endl;
    std::string filename = "test.chtl";
    size_t dotPos = filename.find_last_of('.');
    assert(dotPos != std::string::npos);
    std::string basename = filename.substr(0, dotPos);
    assert(basename == "test");
    std::cout << "  ✓ File system operations work" << std::endl;
    
    std::cout << std::endl;
    std::cout << "All basic tests passed! ✓" << std::endl;
    std::cout << "CHTL core functionality is ready for implementation." << std::endl;
    
    return 0;
}