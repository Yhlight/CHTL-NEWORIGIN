#include "../../Util/StringUtil/StringUtil.h"
#include <iostream>
#include <cassert>
#include <vector>

void testSplit() {
    std::cout << "Testing StringUtil::split..." << std::endl;
    std::vector<std::string> result = CHTL::StringUtil::split("a,b,c", ',');
    assert(result.size() == 3);
    assert(result[0] == "a");
    assert(result[1] == "b");
    assert(result[2] == "c");
    std::cout << "testSplit PASSED." << std::endl;
}

void testTrim() {
    std::cout << "Testing StringUtil::trim..." << std::endl;
    std::string s1 = "  hello  ";
    CHTL::StringUtil::trim(s1);
    assert(s1 == "hello");

    std::string s2 = "world  ";
    CHTL::StringUtil::trim(s2);
    assert(s2 == "world");

    std::string s3 = "  galaxy";
    CHTL::StringUtil::trim(s3);
    assert(s3 == "galaxy");
    std::cout << "testTrim PASSED." << std::endl;
}

void testReplaceAll() {
    std::cout << "Testing StringUtil::replaceAll..." << std::endl;
    std::string s = "hello world, hello universe";
    CHTL::StringUtil::replaceAll(s, "hello", "goodbye");
    assert(s == "goodbye world, goodbye universe");
    std::cout << "testReplaceAll PASSED." << std::endl;
}


int main() {
    std::cout << "--- Running StringUtil Tests ---" << std::endl;
    testSplit();
    testTrim();
    testReplaceAll();
    std::cout << "--------------------------------" << std::endl;
    std::cout << "All StringUtil tests passed!" << std::endl;
    return 0;
}
