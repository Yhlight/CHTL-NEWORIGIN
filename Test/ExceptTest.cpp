#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include <memory>
#include <string>

TEST_CASE("Parser handles 'except' clause correctly", "[integration]") {
    SECTION("Valid case: element not in except list is allowed") {
        const std::string entryFile = "Test/integration/except_valid.chtl";
        CHTLProcessor processor(entryFile);
        REQUIRE_NOTHROW(processor.process());
    }

    SECTION("Invalid case: element in except list throws an error") {
        const std::string entryFile = "Test/integration/except_invalid.chtl";
        CHTLProcessor processor(entryFile);
        REQUIRE_THROWS_AS(processor.process(), std::runtime_error);
    }
}