#pragma once

#include <iostream>
#include <string>

namespace CHTL {
    namespace Test {
        void assertEquals(const std::string& expected, const std::string& actual, const std::string& testName) {
            if (expected != actual) {
                std::cerr << "Test failed: " << testName << std::endl;
                std::cerr << "  Expected: " << expected << std::endl;
                std::cerr << "  Actual:   " << actual << std::endl;
                exit(1);
            }
        }

        template <typename T>
        void assertEquals(T expected, T actual, const std::string& testName) {
            if (expected != actual) {
                std::cerr << "Test failed: " << testName << std::endl;
                std::cerr << "  Expected: " << expected << std::endl;
                std::cerr << "  Actual:   " << actual << std::endl;
                exit(1);
            }
        }
    }
}