#include "gtest/gtest.h"
#include "../src/Util/ImportResolver/ImportResolver.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/CHTL/CHTLNode/RootNode.h"
#include "../src/CHTL/CHTLNode/ImportNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include <fstream>
#include <iostream>

class ImportResolverTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a file to be imported
        std::ofstream imported_file("imported.chtl");
        imported_file << R"(
            div {
                text: "Hello from imported file";
            }
        )";
        imported_file.close();

        // Create the main file that imports the other file
        std::ofstream main_file("main.chtl");
        main_file << R"(
            [Import] @Chtl from "imported.chtl";
        )";
        main_file.close();
    }

    void TearDown() override {
        remove("imported.chtl");
        remove("main.chtl");
    }
};

TEST_F(ImportResolverTest, ResolvesSimpleImport) {
    std::ifstream file("main.chtl");
    ASSERT_TRUE(file.is_open());

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string chtl_code = buffer.str();

    CHTLParser parser(chtl_code);
    parser.parse();

    // Before resolving, the root should have one child: an ImportNode
    BaseNode* root_before = parser.getRoot();
    ASSERT_EQ(root_before->getChildren().size(), 1);
    ASSERT_EQ(root_before->getChildren()[0]->getType(), NodeType::Import);

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    // After resolving, the root should have one child: an ElementNode
    BaseNode* root_after = parser.getRoot();
    ASSERT_EQ(root_after->getChildren().size(), 1);
    ASSERT_EQ(root_after->getChildren()[0]->getType(), NodeType::Element);

    ElementNode* div = static_cast<ElementNode*>(root_after->getChildren()[0].get());
    ASSERT_EQ(div->getTagName(), "div");
}