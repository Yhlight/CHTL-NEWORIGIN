#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include <memory>
#include <string>

TEST_CASE("Processor handles imports and namespaces correctly", "[integration]") {
    // This test simulates a full compilation run with imports.
    // It relies on the test files created in the 'Test/integration' directory.
    const std::string entryFile = "Test/integration/main.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);

    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    // The core of the test: Did the style from the imported module get applied?
    std::string expected_style = "style=\"color: red; font-weight: bold;\"";
    REQUIRE(result.find(expected_style) != std::string::npos);
}