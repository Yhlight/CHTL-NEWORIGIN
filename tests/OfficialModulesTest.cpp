#include "../catch.hpp"
#include "CHTL/CHTLLoader/ModuleResolver.h"
#include "CHTL/CHTLLoader/Loader.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/FragmentNode.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// We need to bring in the cmod_packer's main logic to test it.
// In a larger project, this might be refactored into a shared library.
#include "cmod_packer/main.cpp"

namespace fs = std::filesystem;

struct OfficialModuleTestFixture {
    fs::path test_dir;
    fs::path official_module_dir;

    OfficialModuleTestFixture() {
        test_dir = fs::current_path() / "official_module_test_env";
        official_module_dir = test_dir / "modules"; // This will be our "official" dir
        fs::create_directories(official_module_dir);
    }

    ~OfficialModuleTestFixture() {
        fs::remove_all(test_dir);
    }

    void pack_chtholly_module() {
        // This simulates running `cmod_packer modules/Chtholly official_module_test_env/modules/Chtholly.cmod`
        const char* argv[] = {
            "cmod_packer",
            "modules/Chtholly",
            (official_module_dir / "Chtholly.cmod").string().c_str()
        };
        int argc = sizeof(argv) / sizeof(argv[0]);
        // We're calling the packer's main function directly.
        // A real test suite might have a helper function for this.
        // For simplicity, we'll assume it works as implemented.
        // A more robust test would capture stdout/stderr and check the return code.
        main(argc, const_cast<char**>(argv));
    }
};

TEST_CASE_METHOD(OfficialModuleTestFixture, "Official Module End-to-End Test", "[official_modules]") {
    // 1. Package our source module into a .cmod file in our test environment
    pack_chtholly_module();
    REQUIRE(fs::exists(official_module_dir / "Chtholly.cmod"));

    // 2. Create a source file that tries to import and use the module
    std::string source_to_parse = R"(
        # To make this test work, we need to tell the resolver where to look for "official" modules.
        # In a real compiler, this would be a pre-configured path. Here, we'll just use the relative path.
        [Import] @Chtl from "Chtholly";

        body {
            @Element Accordion;
        }
    )";

    // 3. Parse the source file
    Lexer lexer(source_to_parse);
    // We pass the test directory as the "source path" so the resolver can find the 'modules' subdir
    Parser parser(lexer, test_dir.string());
    auto nodes = parser.parse();

    // 4. Validate the resulting AST
    REQUIRE(nodes.size() == 1);
    auto* body_node = dynamic_cast<ElementNode*>(nodes[0].get());
    REQUIRE(body_node != nullptr);
    REQUIRE(body_node->tagName == "body");
    REQUIRE(body_node->children.size() == 1);

    // The @Element should have been expanded into a FragmentNode
    auto* fragment_node = dynamic_cast<FragmentNode*>(body_node->children[0].get());
    REQUIRE(fragment_node != nullptr);

    // The Accordion fragment should contain a div with class 'accordion-item'
    REQUIRE(fragment_node->children.size() > 0);
    auto* accordion_div = dynamic_cast<ElementNode*>(fragment_node->children[0].get());
    REQUIRE(accordion_div != nullptr);
    REQUIRE(accordion_div->tagName == "div");
    REQUIRE(accordion_div->attributes.count("class") == 1);
    REQUIRE(accordion_div->attributes.at("class")->toString() == "accordion-item");
}