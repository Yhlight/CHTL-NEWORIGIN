#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include <memory>
#include <string>

TEST_CASE("Processor handles use html5 directive", "[integration]") {
    const std::string entryFile = "Test/integration/use_test.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);
    REQUIRE(doc->getHasHtml5Doctype() == true);
}