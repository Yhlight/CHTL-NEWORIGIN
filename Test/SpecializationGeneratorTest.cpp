#include "TestUtils.h"
#include <stdexcept>

TEST_CASE("Generator handles element deletion in specialization", "[generator][specialization]") {
    std::string input = R"(
        [Template] @Element MyComponent {
            p { text { "first" } }
            span { text { "second" } }
            div { text { "third" } }
        }

        body {
            @Element MyComponent {
                delete span;
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == "<body><p>first</p><div>third</div></body>");
}

TEST_CASE("Generator handles element insertion in specialization", "[generator][specialization]") {
    std::string input = R"(
        [Template] @Element MyComponent {
            p { text { "first" } }
            div { text { "third" } }
        }

        body {
            @Element MyComponent {
                insert after p {
                    span { text { "inserted" } }
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == "<body><p>first</p><span>inserted</span><div>third</div></body>");
}

TEST_CASE("Generator handles element modification in specialization", "[generator][specialization]") {
    std::string input = R"(
        [Template] @Element MyComponent {
            p { text { "first" } }
        }

        body {
            @Element MyComponent {
                p {
                    style {
                        color: red;
                    }
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    // Note: The logic for deep-merging style modifications is not fully implemented yet.
    // This test will fail until that logic is added. I will add it in a subsequent step.
    // For now, this test serves as a placeholder for the required functionality.
    REQUIRE(generator.getHtml() == R"(<body><p style="color:red;">first</p></body>)");
}