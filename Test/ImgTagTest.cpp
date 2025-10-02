#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>
#include <string>
#include <algorithm>

// Helper function to normalize whitespace for comparison
std::string normalize_whitespace(const std::string& str) {
    std::string result;
    std::unique_copy(str.begin(), str.end(), std::back_inserter(result),
                     [](char a, char b) { return std::isspace(a) && std::isspace(b); });
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

TEST_CASE("Generator handles img tag correctly", "[generator][img]") {
    const std::string input = R"(
img {
    src: "my-image.png";
    width: 100;
    height: 200;
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);

    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    // We need to find the body content to check the img tag
    std::string body_content;
    size_t body_start = result.find("<body>");
    size_t body_end = result.find("</body>");
    if (body_start != std::string::npos && body_end != std::string::npos) {
        body_start += std::string("<body>").length();
        body_content = result.substr(body_start, body_end - body_start);
    }

    std::string normalized_body = normalize_whitespace(body_content);

    // The attributes might not be in a specific order, so we check for each part
    REQUIRE(normalized_body.find("<img") != std::string::npos);
    REQUIRE(normalized_body.find("src=\"my-image.png\"") != std::string::npos);
    REQUIRE(normalized_body.find("width=\"100\"") != std::string::npos);
    REQUIRE(normalized_body.find("height=\"200\"") != std::string::npos);
    REQUIRE(normalized_body.find("/>") != std::string::npos);
}