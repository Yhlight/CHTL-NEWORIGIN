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

#include "cmod_packer/packer.h"

namespace fs = std::filesystem;

struct OfficialModuleTestFixture {
    fs::path test_dir;
    fs::path official_module_dir;
    fs::path modules_source_dir;

    OfficialModuleTestFixture() {
        test_dir = fs::current_path() / "official_module_test_env";
        official_module_dir = test_dir / "modules"; // This will be our "official" dir for packaged cmods
        modules_source_dir = fs::current_path() / "../../modules"; // Path to the actual source of the modules

        fs::create_directories(official_module_dir);
    }

    ~OfficialModuleTestFixture() {
        fs::remove_all(test_dir);
    }

    void pack_module(const std::string& module_name) {
        fs::path source_path = modules_source_dir / module_name;
        fs::path output_path = official_module_dir / (module_name + ".cmod");
        int result = pack_cmod(source_path, output_path);
        REQUIRE(result == 0);
    }
};

TEST_CASE_METHOD(OfficialModuleTestFixture, "Chtholly Module End-to-End Test", "[official_modules]") {
    // 1. Package the module
    pack_module("Chtholly");
    REQUIRE(fs::exists(official_module_dir / "Chtholly.cmod"));

    // 2. Create a source file that imports and uses the module
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

TEST_CASE_METHOD(OfficialModuleTestFixture, "Yuigahama Module End-to-End Test", "[official_modules]") {
    // 1. Package the module
    pack_module("Yuigahama");
    REQUIRE(fs::exists(official_module_dir / "Yuigahama.cmod"));

    // 2. Create a source file that imports and uses the module
    std::string source_to_parse = R"(
        [Import] @Chtl from "Yuigahama";

        body {
            @Element MusicPlayer {
                audio { src: "my-song.mp3"; }
            }
        }
    )";

    // 3. Parse the source file
    Lexer lexer(source_to_parse);
    Parser parser(lexer, test_dir.string());
    auto nodes = parser.parse();

    // 4. Validate the resulting AST
    REQUIRE(nodes.size() == 1);
    auto* body_node = dynamic_cast<ElementNode*>(nodes[0].get());
    REQUIRE(body_node != nullptr);
    REQUIRE(body_node->children.size() == 1);

    // The @Element should have been expanded into a FragmentNode
    auto* fragment_node = dynamic_cast<FragmentNode*>(body_node->children[0].get());
    REQUIRE(fragment_node != nullptr);

    // The MusicPlayer fragment should contain a div with class 'music-player'
    REQUIRE(fragment_node->children.size() > 0);
    auto* player_div = dynamic_cast<ElementNode*>(fragment_node->children[0].get());
    REQUIRE(player_div != nullptr);
    REQUIRE(player_div->tagName == "div");
    REQUIRE(player_div->attributes.count("class") == 1);
    REQUIRE(player_div->attributes.at("class")->toString() == "music-player");
}