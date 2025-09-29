#include "../catch.hpp"
#include "CJMOD/Syntax.h"
#include "CJMOD/Arg.h"
#include "CJMOD/AtomArg.h"
#include "CJMOD/CJMODScanner.h"
#include "CJMOD/CJMODGenerator.h"

TEST_CASE("CJMOD API: Syntax Class", "[cjmod_api]") {
    SECTION("analyze returns a non-null Arg object") {
        auto arg = Syntax::analyze("$ ** $");
        REQUIRE(arg != nullptr);
    }

    SECTION("Utility functions run without crashing") {
        REQUIRE(Syntax::isObject("{}") == true);
        REQUIRE(Syntax::isObject("{a:1}") == true);
        REQUIRE(Syntax::isFunction("function() {}") == true);
        REQUIRE(Syntax::isArray("[]") == true);
    }
}

TEST_CASE("CJMOD API: AtomArg Class", "[cjmod_api]") {
    SECTION("Correctly parses placeholder flags") {
        AtomArg simple("$");
        REQUIRE(simple.is_optional == false);
        REQUIRE(simple.is_required == false);
        REQUIRE(simple.is_unordered == false);

        AtomArg complex("$?_!");
        REQUIRE(complex.is_optional == true);
        REQUIRE(complex.is_required == true);
        REQUIRE(complex.is_unordered == true);
    }

    SECTION("fillValue and bind work") {
        AtomArg arg("$");
        arg.fillValue("test_value");
        REQUIRE(arg.value == "test_value");

        bool bound_fn_called = false;
        arg.bind([&](const std::string& val) {
            bound_fn_called = true;
            return val;
        });
        arg.bind_function("test");
        REQUIRE(bound_fn_called == true);
    }
}

TEST_CASE("CJMOD API: Arg Class", "[cjmod_api]") {
    SECTION("transform and getTransformation work") {
        Arg arg;
        arg.transform("const a = 1;");
        REQUIRE(arg.getTransformation() == "const a = 1;");
    }
}

TEST_CASE("CJMOD API: Scanner and Generator", "[cjmod_api]") {
    SECTION("Scanner and Generator can be called") {
        // This is just a smoke test to ensure the static methods can be called.
        Arg syntax_arg;
        auto scanned_arg = CJMODScanner::scan(syntax_arg, "**");
        REQUIRE(scanned_arg != nullptr);

        scanned_arg->transform("console.log('hello');");
        CJMODGenerator::exportResult(*scanned_arg);
        SUCCEED(); // Pass if no exceptions are thrown
    }
}