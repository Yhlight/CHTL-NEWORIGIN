#include <catch.hpp>
#include "CHTL/CHTLContext.h"

TEST_CASE("CHTLContext - Basic Functionality", "[context]") {
    CHTL::CHTLContext context;
    context.initialize();
    
    SECTION("Initial state") {
        REQUIRE(!context.isSuccessful());
        REQUIRE(context.getErrorMessage().empty());
        REQUIRE(context.getHtmlOutput().empty());
        REQUIRE(context.getCssOutput().empty());
        REQUIRE(context.getJsOutput().empty());
    }
    
    SECTION("Configuration") {
        context.setConfiguration("DEBUG_MODE", "true");
        REQUIRE(context.getConfiguration("DEBUG_MODE") == "true");
        REQUIRE(context.getConfiguration("NONEXISTENT").empty());
    }
    
    SECTION("Reset") {
        context.setConfiguration("TEST", "value");
        context.reset();
        REQUIRE(context.getConfiguration("TEST").empty());
    }
}

TEST_CASE("CHTLContext - Compilation", "[context]") {
    CHTL::CHTLContext context;
    context.initialize();
    
    SECTION("Empty string compilation") {
        bool success = context.compileString("");
        REQUIRE(success);
        REQUIRE(context.isSuccessful());
    }
    
    SECTION("Simple CHTL compilation") {
        std::string input = "div { text: \"Hello World\"; }";
        bool success = context.compileString(input);
        REQUIRE(success);
        REQUIRE(context.isSuccessful());
    }
}