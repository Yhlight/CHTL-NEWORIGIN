#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>

TEST_CASE("Simple arithmetic operation in style property", "[attribute_operation]") {
    const std::string input = R"(
div {
    style {
        width: 100px + 50px;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    auto doc = parser.parse();
    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    REQUIRE(result.find("style=\"width: 150px;\"") != std::string::npos);
}