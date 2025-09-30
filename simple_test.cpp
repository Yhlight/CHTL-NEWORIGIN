#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Simple test", "[basic]") {
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("String test", "[string]") {
    std::string str = "hello";
    REQUIRE(str == "hello");
}