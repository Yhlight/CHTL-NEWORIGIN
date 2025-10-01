#include "catch.hpp"
#include "CHTLContext.h"

using namespace CHTL;

TEST_CASE("CHTLContext - Basic functionality", "[CHTLContext]") {
    CHTLContext context;
    
    SECTION("Initial state") {
        REQUIRE(context.getInputFile().empty());
        REQUIRE(context.getOutputDirectory().empty());
        REQUIRE_FALSE(context.hasErrors());
        REQUIRE_FALSE(context.isDebugMode());
    }
    
    SECTION("Input file management") {
        std::string testFile = "test.chtl";
        context.setInputFile(testFile);
        REQUIRE(context.getInputFile() == testFile);
    }
    
    SECTION("Output directory management") {
        std::string testDir = "output";
        context.setOutputDirectory(testDir);
        REQUIRE(context.getOutputDirectory() == testDir);
    }
    
    SECTION("Configuration management") {
        context.setConfiguration("debug", "true");
        context.setConfiguration("version", "1.0.0");
        
        REQUIRE(context.getConfiguration("debug") == "true");
        REQUIRE(context.getConfiguration("version") == "1.0.0");
        REQUIRE(context.getConfiguration("nonexistent").empty());
    }
    
    SECTION("Error handling") {
        REQUIRE_FALSE(context.hasErrors());
        
        context.addError("Test error", 10, 5);
        REQUIRE(context.hasErrors());
        REQUIRE(context.getErrors().size() == 1);
        REQUIRE(context.getErrors()[0].find("Test error") != std::string::npos);
        
        context.clearErrors();
        REQUIRE_FALSE(context.hasErrors());
    }
    
    SECTION("Warning handling") {
        context.addWarning("Test warning", 15, 8);
        REQUIRE(context.getWarnings().size() == 1);
        REQUIRE(context.getWarnings()[0].find("Test warning") != std::string::npos);
    }
    
    SECTION("Debug mode") {
        context.setDebugMode(true);
        REQUIRE(context.isDebugMode());
        
        context.setDebugMode(false);
        REQUIRE_FALSE(context.isDebugMode());
    }
    
    SECTION("Module management") {
        context.addModule("test", "module.chtl");
        REQUIRE(context.getModulePath("test") == "module.chtl");
        REQUIRE(context.getModulePath("nonexistent").empty());
    }
}

TEST_CASE("CHTLContext - Error handling edge cases", "[CHTLContext]") {
    CHTLContext context;
    
    SECTION("Multiple errors") {
        context.addError("Error 1");
        context.addError("Error 2", 10);
        context.addError("Error 3", 20, 5);
        
        REQUIRE(context.hasErrors());
        REQUIRE(context.getErrors().size() == 3);
    }
    
    SECTION("Empty error message") {
        context.addError("");
        REQUIRE(context.hasErrors());
        REQUIRE(context.getErrors().size() == 1);
    }
}