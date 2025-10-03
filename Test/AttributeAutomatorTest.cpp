#include <gtest/gtest.h>
#include "Util/AttributeAutomator/AttributeAutomator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ClassSelectorNode.h"
#include "CHTLNode/IdSelectorNode.h"

TEST(AttributeAutomatorTest, AutomatesSingleClass) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->addChild(std::make_unique<ClassSelectorNode>("my-class"));
    root->addChild(std::move(styleNode));

    // Run the automator
    AttributeAutomator automator;
    automator.process(root.get());

    // Verify the result
    auto attributes = root->getAttributes();
    ASSERT_EQ(attributes.size(), 1);
    EXPECT_EQ(attributes["class"], "my-class");
}

TEST(AttributeAutomatorTest, AutomatesMultipleClasses) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    root->setAttribute("class", "existing-class"); // Pre-existing class
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->addChild(std::make_unique<ClassSelectorNode>("new-class"));
    root->addChild(std::move(styleNode));

    // Run the automator
    AttributeAutomator automator;
    automator.process(root.get());

    // Verify the result
    auto attributes = root->getAttributes();
    ASSERT_EQ(attributes.size(), 1);
    EXPECT_EQ(attributes["class"], "existing-class new-class");
}

TEST(AttributeAutomatorTest, AutomatesId) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->addChild(std::make_unique<IdSelectorNode>("my-id"));
    root->addChild(std::move(styleNode));

    // Run the automator
    AttributeAutomator automator;
    automator.process(root.get());

    // Verify the result
    auto attributes = root->getAttributes();
    ASSERT_EQ(attributes.size(), 1);
    EXPECT_EQ(attributes["id"], "my-id");
}

TEST(AttributeAutomatorTest, DoesNotOverwriteExistingId) {
    // Manually construct the AST
    auto root = std::make_unique<ElementNode>("div");
    root->setAttribute("id", "existing-id"); // Pre-existing id
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->addChild(std::make_unique<IdSelectorNode>("new-id"));
    root->addChild(std::move(styleNode));

    // Run the automator
    AttributeAutomator automator;
    automator.process(root.get());

    // Verify the result - the id should not be overwritten
    auto attributes = root->getAttributes();
    ASSERT_EQ(attributes.size(), 1);
    EXPECT_EQ(attributes["id"], "existing-id");
}