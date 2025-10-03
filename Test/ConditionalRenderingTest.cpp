#include "catch.hpp"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/HtmlGenerator.h"
#include "../CHTL/CHTLContext/CHTLContext.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to normalize HTML for comparison by removing all whitespace
static std::string normalizeHtml(const std::string& html) {
    std::string normalized;
    for (char c : html) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            normalized += c;
        }
    }
    return normalized;
}


static void runConditionalTest(const std::string& chtlInput, const std::string& expectedOutput) {
    CHTLContext context;
    CHTLParser parser(chtlInput, context);
    auto document = parser.parse();

    HtmlGenerator generator;
    document->accept(generator);
    std::string result = generator.getResult();

    REQUIRE(normalizeHtml(result) == normalizeHtml(expectedOutput));
}

TEST_CASE("Conditional Rendering: Simple If", "[conditional]") {
    SECTION("True condition") {
        std::string chtl = R"(div { if { condition: 1 > 0, display: "block"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div style="display: block;"></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
    SECTION("False condition") {
        std::string chtl = R"(div { if { condition: 1 < 0, display: "none"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
}

TEST_CASE("Conditional Rendering: If-Else", "[conditional]") {
    SECTION("If branch is taken") {
        std::string chtl = R"(div { if { condition: 1 > 0, color: "red"; } else { color: "blue"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div style="color: red;"></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
    SECTION("Else branch is taken") {
        std::string chtl = R"(div { if { condition: 1 < 0, color: "red"; } else { color: "blue"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div style="color: blue;"></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
}

TEST_CASE("Conditional Rendering: If-Else If-Else", "[conditional]") {
    SECTION("If branch is taken") {
        std::string chtl = R"(div { if { condition: 2 > 1, font-size: "16px"; } else if { condition: 1 > 0, font-size: "14px"; } else { font-size: "12px"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div style="font-size: 16px;"></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
    SECTION("Else If branch is taken") {
        std::string chtl = R"(div { if { condition: 1 > 2, font-size: "16px"; } else if { condition: 1 > 0, font-size: "14px"; } else { font-size: "12px"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div style="font-size: 14px;"></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
    SECTION("Else branch is taken") {
        std::string chtl = R"(div { if { condition: 1 > 2, font-size: "16px"; } else if { condition: 0 > 1, font-size: "14px"; } else { font-size: "12px"; } })";
        std::string expected = R"(<html><head><style></style></head><body><div style="font-size: 12px;"></div></body></html>)";
        runConditionalTest(chtl, expected);
    }
}

TEST_CASE("Conditional Rendering: Attribute Reference", "[conditional]") {
    std::string chtl = R"(div { width: "100px"; if { condition: width > 50, height: "200px"; } })";
    std::string expected = R"(<html><head><style></style></head><body><div width="100px" style="height: 200px;"></div></body></html>)";
    runConditionalTest(chtl, expected);
}