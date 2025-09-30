#include "../catch.hpp"
#include "CJMOD/Syntax.h"
#include "CJMOD/Arg.h"
#include "CJMOD/AtomArg.h"
#include "CJMOD/CJMODScanner.h"
#include "CJMOD/CJMODGenerator.h"
#include "CHTL/CHTLManage/CJMODManager.h"
#include "CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.h"

TEST_CASE("CJMOD API: Real Scanner Logic", "[cjmod_api][scanner]") {
    // 1. Create a token stream representing: const x = a ** b;
    std::vector<CHTLJSToken> tokens = {
        {CHTLJSTokenType::Identifier, "const"},
        {CHTLJSTokenType::Identifier, "x"},
        {CHTLJSTokenType::Identifier, "="},
        {CHTLJSTokenType::Identifier, "a"},
        {CHTLJSTokenType::Operator, "**"}, // This is the keyword at index 4
        {CHTLJSTokenType::Identifier, "b"},
        {CHTLJSTokenType::Semicolon, ";"}
    };

    // 2. Define the syntax pattern we are looking for: $ ** $
    auto syntax_arg = Syntax::analyze("$ ** $");

    // 3. Scan the token stream
    auto result = CJMODScanner::scan(tokens, *syntax_arg, "**", 0);

    // 4. Verify the results
    REQUIRE(result.has_value());

    // The match should be at indices 3, 4, 5
    REQUIRE(result->start_index == 3);
    REQUIRE(result->end_index == 5);

    // Check the captured values
    REQUIRE(result->matched_arg != nullptr);
    REQUIRE(result->matched_arg->arguments.size() == 3);
    REQUIRE(result->matched_arg->arguments[0]->value == "a");
    REQUIRE(result->matched_arg->arguments[1]->value == "**");
    REQUIRE(result->matched_arg->arguments[2]->value == "b");
}


TEST_CASE("CJMOD API: End-to-end Compilation with Real Scanner", "[cjmod_api][e2e]") {
    // 1. Define a simple CJMOD module for the power operator (**)
    auto power_module = std::make_unique<CJMODModule>();
    power_module->name = "PowerOperator";
    power_module->keyword = "**";
    power_module->syntax_arg = Syntax::analyze("$ ** $");
    power_module->processFunc = [](Arg& arg) {
        std::string base = arg.arguments[0]->value;
        std::string exp = arg.arguments[2]->value;
        arg.transform("Math.pow(" + base + ", " + exp + ")");
        CJMODGenerator::exportResult(arg);
    };

    // 2. Load the module into the manager
    CJMODManager cjmodManager;
    cjmodManager.load_module(std::move(power_module));

    // 3. Define the source code to be compiled. The real scanner needs to handle surrounding tokens.
    std::string source = "const result = 3 ** 4;";

    // 4. Compile the source using the CHTLJSCompiler
    CHTLJSCompiler compiler;
    std::string final_js = compiler.compile(source, cjmodManager);

    // 5. Assert that the transformation was applied correctly
    std::string expected_js = "const result = Math.pow(3, 4);";
    REQUIRE(final_js == expected_js);
}


TEST_CASE("CJMOD API: Syntax Class", "[cjmod_api]") {
    SECTION("analyze returns a non-null Arg object") {
        auto arg = Syntax::analyze("$ ** $");
        REQUIRE(arg != nullptr);
        REQUIRE(arg->arguments.size() == 3);
        REQUIRE(arg->arguments[0]->placeholder == "$");
        REQUIRE(arg->arguments[1]->placeholder == "**");
        REQUIRE(arg->arguments[2]->placeholder == "$");
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
        Arg syntax_arg;
        std::vector<CHTLJSToken> tokens;
        auto scanned_arg = CJMODScanner::scan(tokens, syntax_arg, "**", 0);
        REQUIRE(!scanned_arg.has_value()); // Expect no result on empty tokens
    }
}