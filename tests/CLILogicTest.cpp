#include "../catch.hpp"
#include "third-party/cxxopts/cxxopts.hpp"
#include <vector>
#include <string>

// This helper function mimics the setup in main.cpp to test argument parsing.
cxxopts::parse_result parse_test_args(int argc, const char* argv[]) {
    cxxopts::Options options("chtl_compiler_test", "A test for the CHTL CLI");
    options.add_options()
        ("h,help", "Print usage information")
        ("o,output", "Output directory for compiled files", cxxopts::value<std::string>()->default_value("."))
        ("inline", "Inline both CSS and JS into a single HTML file")
        ("inline-css", "Inline only CSS into the HTML file")
        ("inline-js", "Inline only JS into the HTML file")
        ("default-struct", "Wrap the output in a default html/head/body structure")
        ("input", "Input CHTL file", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"input"});
    return options.parse(argc, argv);
}

TEST_CASE("CLI Logic: Basic Arguments", "[cli]") {
    const char* argv[] = {"chtl_compiler_test", "my_file.chtl"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    auto result = parse_test_args(argc, argv);

    REQUIRE(result.count("input") == 1);
    REQUIRE(result["input"].as<std::vector<std::string>>()[0] == "my_file.chtl");
    REQUIRE(result.count("output") == 1); // Because it has a default value
    REQUIRE(result["output"].as<std::string>() == ".");
}

TEST_CASE("CLI Logic: Output Flag", "[cli]") {
    SECTION("Long flag") {
        const char* argv[] = {"chtl_compiler_test", "--output", "dist", "my_file.chtl"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        auto result = parse_test_args(argc, argv);
        REQUIRE(result["output"].as<std::string>() == "dist");
    }
    SECTION("Short flag") {
        const char* argv[] = {"chtl_compiler_test", "-o", "build", "my_file.chtl"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        auto result = parse_test_args(argc, argv);
        REQUIRE(result["output"].as<std::string>() == "build");
    }
}

TEST_CASE("CLI Logic: Help Flag", "[cli]") {
    SECTION("Long flag") {
        const char* argv[] = {"chtl_compiler_test", "--help"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        auto result = parse_test_args(argc, argv);
        REQUIRE(result.count("help") == 1);
    }
    SECTION("Short flag") {
        const char* argv[] = {"chtl_compiler_test", "-h"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        auto result = parse_test_args(argc, argv);
        REQUIRE(result.count("help") == 1);
    }
}

TEST_CASE("CLI Logic: Boolean Flags", "[cli]") {
    const char* argv[] = {"chtl_compiler_test", "--inline-css", "--default-struct", "my_file.chtl"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    auto result = parse_test_args(argc, argv);

    REQUIRE(result.count("inline-css") == 1);
    REQUIRE(result.count("default-struct") == 1);
    REQUIRE(result.count("inline") == 0);
    REQUIRE(result.count("inline-js") == 0);
}

TEST_CASE("CLI Logic: Combined Inline Flag", "[cli]") {
    const char* argv[] = {"chtl_compiler_test", "--inline", "my_file.chtl"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    auto result = parse_test_args(argc, argv);

    // In main.cpp, --inline implies both --inline-css and --inline-js
    bool inline_css = result.count("inline-css") || result.count("inline");
    bool inline_js = result.count("inline-js") || result.count("inline");

    REQUIRE(result.count("inline") == 1);
    REQUIRE(inline_css == true);
    REQUIRE(inline_js == true);
}

TEST_CASE("CLI Logic: No Input File", "[cli]") {
    const char* argv[] = {"chtl_compiler_test", "-o", "dist"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    auto result = parse_test_args(argc, argv);

    REQUIRE(result.count("input") == 0);
}