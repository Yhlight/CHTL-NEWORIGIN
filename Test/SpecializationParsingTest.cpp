#include "TestUtils.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TemplateUsageNode.h"
#include "../CHTL/CHTLNode/DeleteNode.h"
#include "../CHTL/CHTLNode/InsertNode.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"
#include <memory>
#include <stdexcept>

// Helper to get the specialization root node from a template usage
static std::shared_ptr<CHTL::BaseNode> getSpecializationRoot(const std::string& input) {
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens, configManager);
    auto root = parser.parse();
    REQUIRE(root != nullptr);
    REQUIRE(root->getChildren().size() == 1);
    auto container = std::dynamic_pointer_cast<CHTL::ElementNode>(root->getChildren()[0]);
    REQUIRE(container != nullptr);
    REQUIRE(container->getChildren().size() == 1);
    auto usageNode = std::dynamic_pointer_cast<CHTL::TemplateUsageNode>(container->getChildren()[0]);
    REQUIRE(usageNode != nullptr);
    return usageNode->getSpecialization();
}


TEST_CASE("Parser handles delete rule in specialization", "[parser][specialization]") {
    std::string input = R"(
        div {
            @Element MyTemplate {
                delete prop1, element2;
            }
        }
    )";
    auto specializationRoot = getSpecializationRoot(input);
    REQUIRE(specializationRoot != nullptr);
    REQUIRE(specializationRoot->getChildren().size() == 1);

    auto deleteNode = std::dynamic_pointer_cast<CHTL::DeleteNode>(specializationRoot->getChildren()[0]);
    REQUIRE(deleteNode != nullptr);
    REQUIRE(deleteNode->targets.size() == 2);
    REQUIRE(deleteNode->targets[0] == "prop1");
    REQUIRE(deleteNode->targets[1] == "element2");
}

TEST_CASE("Parser handles insert rule in specialization", "[parser][specialization]") {
    std::string input = R"(
        div {
            @Element MyTemplate {
                insert after .selector {
                    p { text { "new" } }
                }
            }
        }
    )";
    auto specializationRoot = getSpecializationRoot(input);
    REQUIRE(specializationRoot != nullptr);
    REQUIRE(specializationRoot->getChildren().size() == 1);

    auto insertNode = std::dynamic_pointer_cast<CHTL::InsertNode>(specializationRoot->getChildren()[0]);
    REQUIRE(insertNode != nullptr);
    REQUIRE(insertNode->position == CHTL::InsertPosition::AFTER);
    REQUIRE(insertNode->target_selector == ".selector");
    REQUIRE(insertNode->getChildren().size() == 1);
    auto p_node = std::dynamic_pointer_cast<CHTL::ElementNode>(insertNode->getChildren()[0]);
    REQUIRE(p_node != nullptr);
    REQUIRE(p_node->getTagName() == "p");
}