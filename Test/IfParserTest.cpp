#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/IfNode.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>

TEST_CASE("Parser handles if statements", "[parser][if]") {
    const std::string input = R"(
div {
    if {
        condition: width > 100px,
        display: block;
        color: red;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);

    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);
    REQUIRE(root->getTagName() == "div");

    const auto& rootChildren = root->getChildren();
    REQUIRE(rootChildren.size() == 1);

    IfNode* ifNode = dynamic_cast<IfNode*>(rootChildren[0].get());
    REQUIRE(ifNode != nullptr);

    const auto& condition = ifNode->getConditionTokens();
    REQUIRE(condition.size() == 3);
    REQUIRE(condition[0].value == "width");
    REQUIRE(condition[1].type == TokenType::GreaterThan);
    REQUIRE(condition[2].value == "100px");

    const auto& props = ifNode->getProperties();
    REQUIRE(props.size() == 2);
    REQUIRE(props[0].first == "display");
    REQUIRE(std::holds_alternative<std::string>(props[0].second));
    REQUIRE(std::get<std::string>(props[0].second) == "block");
    REQUIRE(props[1].first == "color");
    REQUIRE(std::holds_alternative<std::string>(props[1].second));
    REQUIRE(std::get<std::string>(props[1].second) == "red");
}

TEST_CASE("Parser handles if-else statements", "[parser][if]") {
    const std::string input = R"(
div {
    if {
        condition: width > 100px,
        display: block;
    } else {
        display: none;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    const auto& rootChildren = root->getChildren();
    REQUIRE(rootChildren.size() == 1);
    IfNode* ifNode = dynamic_cast<IfNode*>(rootChildren[0].get());
    REQUIRE(ifNode != nullptr);

    // Check the 'else' branch
    const auto& elseBranch = ifNode->getElseBranch();
    REQUIRE(elseBranch != nullptr);
    IfNode* elseNode = dynamic_cast<IfNode*>(elseBranch.get());
    REQUIRE(elseNode != nullptr);
    REQUIRE(elseNode->getConditionTokens().empty()); // Else has no condition

    const auto& elseProps = elseNode->getProperties();
    REQUIRE(elseProps.size() == 1);
    REQUIRE(elseProps[0].first == "display");
    REQUIRE(std::get<std::string>(elseProps[0].second) == "none");
}

TEST_CASE("Parser handles if-else-if-else statements", "[parser][if]") {
    const std::string input = R"(
div {
    if {
        condition: width > 200px,
        font-size: 20px;
    } else if {
        condition: width > 100px,
        font-size: 16px;
    } else {
        font-size: 12px;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    const auto& rootChildren = root->getChildren();
    REQUIRE(rootChildren.size() == 1);
    IfNode* ifNode = dynamic_cast<IfNode*>(rootChildren[0].get());
    REQUIRE(ifNode != nullptr);

    // Check the 'else if' branch
    const auto& elseIfBranch = ifNode->getElseBranch();
    REQUIRE(elseIfBranch != nullptr);
    IfNode* elseIfNode = dynamic_cast<IfNode*>(elseIfBranch.get());
    REQUIRE(elseIfNode != nullptr);

    const auto& elseIfCondition = elseIfNode->getConditionTokens();
    REQUIRE(elseIfCondition.size() == 3);
    REQUIRE(elseIfCondition[0].value == "width");
    REQUIRE(elseIfCondition[2].value == "100px");

    const auto& elseIfProps = elseIfNode->getProperties();
    REQUIRE(elseIfProps.size() == 1);
    REQUIRE(elseIfProps[0].first == "font-size");
    REQUIRE(std::get<std::string>(elseIfProps[0].second) == "16px");

    // Check the final 'else' branch
    const auto& finalElseBranch = elseIfNode->getElseBranch();
    REQUIRE(finalElseBranch != nullptr);
    IfNode* finalElseNode = dynamic_cast<IfNode*>(finalElseBranch.get());
    REQUIRE(finalElseNode != nullptr);
    REQUIRE(finalElseNode->getConditionTokens().empty());

    const auto& finalElseProps = finalElseNode->getProperties();
    REQUIRE(finalElseProps.size() == 1);
    REQUIRE(finalElseProps[0].first == "font-size");
    REQUIRE(std::get<std::string>(finalElseProps[0].second) == "12px");
}