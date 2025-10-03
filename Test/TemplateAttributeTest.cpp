#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include <memory>
#include <string>

TEST_CASE("Template element attributes are preserved", "[integration]") {
    const std::string entryFile = "Test/integration/template_attribute_test.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);

    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    // Check that the attributes from the template's root element are present
    std::string expected_class = "class=\"component\"";
    std::string expected_id = "id=\"my-component\"";
    REQUIRE(result.find(expected_class) != std::string::npos);
    REQUIRE(result.find(expected_id) != std::string::npos);
}