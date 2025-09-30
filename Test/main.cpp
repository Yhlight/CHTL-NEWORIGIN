#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// Include a header from the CHTL library to test linkage
#include "CHTLLexer/CHTLLexer.h"

TEST_CASE("Basic Assertion", "[main]") {
    REQUIRE(1 == 1);
}

TEST_CASE("Compiler Library Linkage", "[cmake]") {
    // This test will only compile if the test executable is correctly linked
    // against the chtl_lib library.
    SUCCEED("CHTLLexer can be instantiated.");
    CHTL::CHTLLexer lexer;
}