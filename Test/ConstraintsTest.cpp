#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include <stdexcept>

TEST_CASE("Parser handles type and global constraints", "[constraints]") {
    SECTION("Valid type constraint") {
        const std::string entryFile = "Test/integration/type_constraint_valid.chtl";
        CHTLProcessor processor(entryFile);
        REQUIRE_NOTHROW(processor.process());
    }

    SECTION("Invalid type constraint") {
        const std::string entryFile = "Test/integration/type_constraint_invalid.chtl";
        CHTLProcessor processor(entryFile);
        REQUIRE_THROWS_AS(processor.process(), std::runtime_error);
    }

    SECTION("Valid global constraint") {
        const std::string entryFile = "Test/integration/global_constraint_valid.chtl";
        CHTLProcessor processor(entryFile);
        REQUIRE_NOTHROW(processor.process());
    }

    SECTION("Invalid global constraint") {
        const std::string entryFile = "Test/integration/global_constraint_invalid.chtl";
        CHTLProcessor processor(entryFile);
        REQUIRE_THROWS_AS(processor.process(), std::runtime_error);
    }
}