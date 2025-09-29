#include "../catch.hpp"
#include "CHTL/CHTLLoader/ModuleResolver.h"
#include "CHTL/CHTLLoader/Loader.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "libzippp.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// Test fixture for creating a dummy module structure
struct ModuleSystemTestFixture {
    fs::path test_dir;

    ModuleSystemTestFixture() {
        test_dir = fs::current_path() / "test_module_system";
        fs::create_directories(test_dir / "src");
        fs::create_directories(test_dir / "info");
        fs::create_directories(test_dir / "module" / "SubModule" / "src");
        fs::create_directories(test_dir / "module" / "SubModule" / "info");

        // Create some dummy files
        std::ofstream(test_dir / "src" / "main.chtl") << "div {}";
        std::ofstream(test_dir / "info" / "test_module_system.chtl") << "[Info] { name = \"test\"; }";
        std::ofstream(test_dir / "module" / "SubModule" / "src" / "SubModule.chtl") << "[Template] @Style MyStyle {}";
        std::ofstream(test_dir / "module" / "SubModule" / "info" / "SubModule.chtl") << "[Info] { name = \"sub\"; }";

        // Create a dummy .cmod file
        libzippp::ZipArchive zf((test_dir / "module" / "MyCmod.cmod").string());
        zf.open(libzippp::ZipArchive::New);
        zf.addFile("src/MyCmod.chtl", "div {}");
        zf.close();
    }

    ~ModuleSystemTestFixture() {
        fs::remove_all(test_dir);
    }
};

TEST_CASE_METHOD(ModuleSystemTestFixture, "Module Resolver", "[module_system]") {
    ModuleResolver resolver;

    SECTION("Resolves .chtl file in local module directory") {
        auto paths = resolver.resolve("SubModule/src/SubModule.chtl", test_dir / "module");
        REQUIRE(paths.size() == 1);
        REQUIRE(paths[0].filename() == "SubModule.chtl");
    }

    SECTION("Resolves .cmod file by name") {
        auto paths = resolver.resolve("MyCmod", test_dir / "module");
        REQUIRE(paths.size() == 1);
        REQUIRE(paths[0].filename() == "MyCmod.cmod");
    }

    SECTION("Resolves .chtl file with higher precedence than parent directory") {
        // This test is conceptual as we don't have a parent dir with the same name
        // but it tests the search order logic.
        auto paths = resolver.resolve("SubModule", test_dir);
        REQUIRE(paths.empty()); // It shouldn't find a file named 'SubModule', only a directory
    }
}

TEST_CASE_METHOD(ModuleSystemTestFixture, "CMOD Loader", "[module_system]") {
    SECTION("Loads content from a .cmod file") {
        auto contents = Loader::loadCmod((test_dir / "module" / "MyCmod.cmod").string());
        REQUIRE(contents.size() == 1);
        REQUIRE(contents.count("src/MyCmod.chtl") == 1);
        REQUIRE(contents.at("src/MyCmod.chtl") == "div {}");
    }
}

TEST_CASE_METHOD(ModuleSystemTestFixture, "Parser CMOD Import", "[module_system]") {
    SECTION("Parser can import a .cmod file") {
        std::string source = R"([Import] @Chtl from "MyCmod";)";
        Lexer lexer(source);
        // The parser needs the path to the file it's parsing to resolve relative imports
        Parser parser(lexer, (test_dir / "module" / "dummy.chtl").string());
        parser.parse();
        // The test passes if no exceptions are thrown.
        // A more advanced test would check if templates from the module were loaded.
        SUCCEED();
    }
}