#include <gtest/gtest.h>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLContext.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to remove all whitespace for robust comparison.
static std::string removeWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}


TEST(IntegrationTest, FullCompilationSimple) {
    std::string source = R"(
        div {
            id: "main";
            p {
                text { "Hello" }
            }
            # generator comment
        }
    )";

    std::string expected_html = R"(
        <div id="main">
          <p>
            Hello
          </p>
          <!-- generator comment -->
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);

    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesElementCustomization) {
    std::string source = R"(
        [Template] @Element MyComponent {
            h1 { text: "Title"; }
            p { text: "First paragraph."; }
            p { text: "Second paragraph."; }
            span { text: "A span."; }
        }

        div {
            @Element MyComponent {
                delete p[0]; // Delete the first p tag
                delete span;   // Delete the span tag
            }
        }
    )";

    std::string expected_html = R"(
        <div>
            <h1>Title</h1>
            <p>Secondparagraph.</p>
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesElementTemplate) {
    std::string source = R"(
        [Template] @Element MyCard {
            div { class: "card-body"; }
        }

        div {
            class: "card";
            @Element MyCard;
        }
    )";

    std::string expected_html = R"(
        <div class="card">
            <div class="card-body"></div>
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesStyleTemplate) {
    std::string source = R"(
        [Template] @Style DefaultBox {
            width: 100px;
            height: 100px;
        }

        div {
            style {
                background-color: red;
                @Style DefaultBox;
            }
        }
    )";

    std::string expected_html = R"(
        <div style="background-color:red;width:100px;height:100px;">
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesFileImport) {
    std::string source = R"(
        [Import] @Chtl from "../imports/base.chtl"

        p {
            style {
                @Style BaseStyles;
            }
        }
    )";

    std::string expected_html = R"(
        <p style="color:green;font-weight:bold;"></p>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}


TEST(CodeMergerTest, MergesScriptIntoHtml) {
    std::string source = R"(
        div {
            script {
                const my_div = {{.my-class}};
                my_div.textContent = "Hello from JS!";
            }
        }
    )";

    std::string expected_html = R"(
        <div>
            <script>const my_div = document.querySelector('.my-class'); my_div.textContent = "Hello from JS!";</script>
        </div>
    )";

    CHTL::CHTLUnifiedScanner scanner(source);
    CHTL::ScannedFragments fragments = scanner.scan();

    CHTL::CompilerDispatcher dispatcher(fragments);
    std::string result_html = dispatcher.dispatch();

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}


TEST(IntegrationTest, HandlesStyleBlocks) {
    std::string source = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
        }
    )";

    std::string expected_html = R"(
        <div style="color:red;font-size:16px;">
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, HandlesPropertyArithmetic) {
    std::string source = R"(
        div {
            style {
                width: 100px + 50px;
                height: 2 * 25px;
            }
        }
    )";

    std::string expected_html = R"(
        <div style="width:150px;height:50px;">
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}

TEST(IntegrationTest, TextAttributeIsEquivalentToTextBlock) {
    std::string source_attr = R"(div { text: "hello"; })";
    std::string source_block = R"(div { text { "hello" } })";

    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLLexer lexer_attr(source_attr);
    CHTL::CHTLParser parser_attr(lexer_attr.getAllTokens(), context);
    auto ast_attr = parser_attr.parse();
    CHTL::CHTLGenerator generator;
    std::string result_attr = generator.generate(*ast_attr);

    CHTL::CHTLLexer lexer_block(source_block);
    CHTL::CHTLParser parser_block(lexer_block.getAllTokens(), context);
    auto ast_block = parser_block.parse();
    std::string result_block = generator.generate(*ast_block);

    EXPECT_EQ(removeWhitespace(result_attr), removeWhitespace(result_block));
    EXPECT_EQ(removeWhitespace(result_attr), removeWhitespace("<div>hello</div>"));
}


TEST(IntegrationTest, HandlesVoidElements) {
    std::string source = R"(
        div {
            img { src: "image.jpg"; }
            br {}
        }
    )";

    std::string expected_html = R"(
        <div>
            <img src="image.jpg">
            <br>
        </div>
    )";

    CHTL::CHTLLexer lexer(source);
    auto context = std::make_shared<CHTL::CHTLContext>();
    CHTL::CHTLParser parser(lexer.getAllTokens(), context);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result_html = generator.generate(*ast);

    EXPECT_EQ(removeWhitespace(result_html), removeWhitespace(expected_html));
}
