#include "gtest/gtest.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/TemplateDefinitionNode.h"
#include "../src/CHTL/CHTLNode/StyleNode.h"
#include "../src/CHTL/CHTLNode/TemplateUsageNode.h"
#include "../src/Util/TemplateEvaluator/TemplateEvaluator.h"

class TemplateTest : public ::testing::Test {
protected:
    CHTLParser createParser(const std::string& input) {
        return CHTLParser(input);
    }
};

TEST_F(TemplateTest, ParsesTemplateDefinition) {
    std::string chtl_code = R"(
        [Template] @Style DefaultText {
            color: "black";
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 1);

    auto* defNode = dynamic_cast<TemplateDefinitionNode*>(root->getChildren()[0].get());
    ASSERT_NE(defNode, nullptr);
    ASSERT_EQ(defNode->getTemplateType(), "Style");
    ASSERT_EQ(defNode->getTemplateName(), "DefaultText");
}

TEST_F(TemplateTest, EvaluatesStyleTemplate) {
    std::string chtl_code = R"(
        [Template] @Style DefaultText {
            color: "black";
            font-size: 16px;
        }

        div {
            style {
                @Style DefaultText;
            }
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    TemplateEvaluator evaluator;
    evaluator.evaluate(root);

    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 2); // Definition and the div

    auto* divNode = dynamic_cast<ElementNode*>(root->getChildren()[1].get());
    ASSERT_NE(divNode, nullptr);

    const StyleNode* styleNode = nullptr;
    for (const auto& child : divNode->getChildren()) {
        if (child->getType() == NodeType::Style) {
            styleNode = static_cast<const StyleNode*>(child.get());
            break;
        }
    }
    ASSERT_NE(styleNode, nullptr);
    ASSERT_EQ(styleNode->getChildren().size(), 2); // two properties from template
}

TEST_F(TemplateTest, EvaluatesElementTemplate) {
    std::string chtl_code = R"(
        [Template] @Element MyBox {
            div {
                text { "I am in a box" }
            }
        }

        body {
            @Element MyBox;
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    TemplateEvaluator evaluator;
    evaluator.evaluate(root);

    ASSERT_NE(root, nullptr);
    ASSERT_EQ(root->getChildren().size(), 2);

    auto* bodyNode = dynamic_cast<ElementNode*>(root->getChildren()[1].get());
    ASSERT_NE(bodyNode, nullptr);
    ASSERT_EQ(bodyNode->getTagName(), "body");

    ASSERT_EQ(bodyNode->getChildren().size(), 1);
    auto* divNode = dynamic_cast<ElementNode*>(bodyNode->getChildren()[0].get());
    ASSERT_NE(divNode, nullptr);
    ASSERT_EQ(divNode->getTagName(), "div");
}

TEST_F(TemplateTest, ParsesElementTemplateUsage) {
    std::string chtl_code = R"(
        body {
            @Element MyBox;
        }
    )";
    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    ASSERT_NE(root, nullptr);
    auto* bodyNode = dynamic_cast<ElementNode*>(root->getChildren()[0].get());
    ASSERT_NE(bodyNode, nullptr);
    ASSERT_EQ(bodyNode->getChildren().size(), 1);
    auto* usageNode = dynamic_cast<TemplateUsageNode*>(bodyNode->getChildren()[0].get());
    ASSERT_NE(usageNode, nullptr);
    ASSERT_EQ(usageNode->getTemplateType(), "Element");
    ASSERT_EQ(usageNode->getTemplateName(), "MyBox");
}