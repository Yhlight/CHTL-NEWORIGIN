#include <iostream>
#include <cassert>
#include "../Util/FileSystem/FileSystem.h"
#include "../Util/StringUtil/StringUtil.h"
#include "../Util/ZipUtil/ZipUtil.h"

using namespace CHTL::Util;

int main() {
    std::cout << "Running CHTL Utility Tests..." << std::endl;
    
    // Test StringUtil
    std::string testString = "  hello world  ";
    std::string trimmed = StringUtil::trim(testString);
    assert(trimmed == "hello world");
    std::cout << "✓ StringUtil::trim test passed" << std::endl;
    
    // Test string splitting
    std::vector<std::string> parts = StringUtil::split("a,b,c", ',');
    assert(parts.size() == 3);
    assert(parts[0] == "a" && parts[1] == "b" && parts[2] == "c");
    std::cout << "✓ StringUtil::split test passed" << std::endl;
    
    // Test string replacement
    std::string replaced = StringUtil::replaceAll("hello world", "world", "CHTL");
    assert(replaced == "hello CHTL");
    std::cout << "✓ StringUtil::replaceAll test passed" << std::endl;
    
    // Test FileSystem
    std::string currentDir = FileSystem::getCurrentDirectory();
    assert(!currentDir.empty());
    std::cout << "✓ FileSystem::getCurrentDirectory test passed" << std::endl;
    
    // Test path operations
    std::string joined = FileSystem::joinPath("base", "relative");
    assert(joined.find("base") != std::string::npos);
    assert(joined.find("relative") != std::string::npos);
    std::cout << "✓ FileSystem::joinPath test passed" << std::endl;
    
    std::cout << "All utility tests passed!" << std::endl;
    return 0;
}