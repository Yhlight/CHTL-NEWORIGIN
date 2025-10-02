#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include <memory>
#include <string>

TEST_CASE("Processor handles imports and namespaces correctly", "[integration]") {
    // This test simulates a full compilation run with imports.
    // It relies on the test files created in the 'Test/integration' directory.
    const std::string entryFile = "Test/integration/main.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 2); // The import node and the div node

    auto* divNode = dynamic_cast<ElementNode*>(children[1].get());
    REQUIRE(divNode != nullptr);

    // The core of the test: Did the style from the imported module get applied?
    std::string output = divNode->toString();
    // This assertion will fail until the context can resolve qualified names.
    REQUIRE(output.find("/* Style Block: color: red; font-weight: bold; */") != std::string::npos);
}