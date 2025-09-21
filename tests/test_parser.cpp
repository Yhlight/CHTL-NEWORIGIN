#include <gtest/gtest.h>
#include "CHTLCommon.h"
#include "ASTNode.h"

using namespace CHTL;

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // TODO: Initialize parser when implemented
    }
    
    void TearDown() override {
        // TODO: Cleanup parser when implemented
    }
};

TEST_F(ParserTest, BasicASTNodeCreation) {
    auto node = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    
    EXPECT_EQ(node->getType(), NodeType::ELEMENT);
    EXPECT_EQ(node->getValue(), "div");
    EXPECT_TRUE(node->isLeaf());
}

TEST_F(ParserTest, ASTNodeChildren) {
    auto parent = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    auto child = std::make_shared<ASTNode>(NodeType::TEXT, "Hello");
    
    parent->addChild(child);
    
    EXPECT_EQ(parent->getChildCount(), 1);
    EXPECT_FALSE(parent->isLeaf());
    EXPECT_TRUE(parent->isContainer());
    
    auto retrievedChild = parent->getChild(0);
    EXPECT_EQ(retrievedChild->getValue(), "Hello");
}

TEST_F(ParserTest, ElementNode) {
    auto element = std::make_shared<ElementNode>("div");
    
    EXPECT_EQ(element->getTagName(), "div");
    EXPECT_EQ(element->getType(), NodeType::ELEMENT);
    
    element->setAttribute("id", "test");
    EXPECT_TRUE(element->hasAttribute("id"));
    EXPECT_EQ(element->getAttribute("id"), "test");
}

TEST_F(ParserTest, TextNode) {
    auto text = std::make_shared<TextNode>("Hello World", true);
    
    EXPECT_EQ(text->getContent(), "Hello World");
    EXPECT_TRUE(text->isLiteralContent());
    EXPECT_EQ(text->getType(), NodeType::TEXT);
}

TEST_F(ParserTest, StyleNode) {
    auto style = std::make_shared<StyleNode>(false);
    
    style->setProperty("width", "100px");
    style->setProperty("height", "200px");
    
    EXPECT_TRUE(style->hasProperty("width"));
    EXPECT_EQ(style->getProperty("width"), "100px");
    EXPECT_EQ(style->getProperty("height"), "200px");
    
    style->addSelector(".container");
    EXPECT_TRUE(style->hasSelector(".container"));
}

TEST_F(ParserTest, TemplateNode) {
    auto template_node = std::make_shared<TemplateNode>("MyTemplate", "Style", false);
    
    EXPECT_EQ(template_node->getTemplateName(), "MyTemplate");
    EXPECT_EQ(template_node->getTemplateType(), "Style");
    EXPECT_FALSE(template_node->isCustomTemplate());
    
    template_node->setParameter("color", "red");
    EXPECT_TRUE(template_node->hasParameter("color"));
    EXPECT_EQ(template_node->getParameter("color"), "red");
}

TEST_F(ParserTest, ASTNodeValidation) {
    auto validNode = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    EXPECT_TRUE(validNode->isValid());
    
    auto errors = validNode->validate();
    EXPECT_TRUE(errors.empty());
}

TEST_F(ParserTest, ASTNodeSearch) {
    auto parent = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    auto child1 = std::make_shared<ASTNode>(NodeType::TEXT, "Hello");
    auto child2 = std::make_shared<ASTNode>(NodeType::ELEMENT, "span");
    
    parent->addChild(child1);
    parent->addChild(child2);
    
    auto foundText = parent->findFirst(NodeType::TEXT);
    EXPECT_NE(foundText, nullptr);
    EXPECT_EQ(foundText->getValue(), "Hello");
    
    auto foundSpan = parent->findFirstWithValue("span");
    EXPECT_NE(foundSpan, nullptr);
    EXPECT_EQ(foundSpan->getType(), NodeType::ELEMENT);
}

TEST_F(ParserTest, MetadataHandling) {
    auto node = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    
    node->setMetadata("line", "10");
    node->setMetadata("column", "5");
    
    EXPECT_TRUE(node->hasMetadata("line"));
    EXPECT_EQ(node->getMetadata("line"), "10");
    EXPECT_EQ(node->getMetadata("column"), "5");
}