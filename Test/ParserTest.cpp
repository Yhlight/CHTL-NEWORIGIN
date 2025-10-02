#include <gtest/gtest.h>
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"

TEST(ParserTest, ParsesSingleEmptyElement) {
    std::string input = "div {}";
    CHTLParser parser(input);
    parser.parse();

    BaseNode* root = parser.getRoot();
    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 1);

    BaseNode* element = root->getChildren()[0].get();
    ASSERT_EQ(element->getType(), NodeType::Element);

    auto* elementNode = static_cast<ElementNode*>(element);
    EXPECT_EQ(elementNode->getTagName(), "div");
}