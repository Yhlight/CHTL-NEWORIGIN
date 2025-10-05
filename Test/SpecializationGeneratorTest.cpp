#include "TestUtils.h"
#include <stdexcept>

TEST_CASE("Generator handles indexed element deletion in specialization", "[generator][specialization]") {
    std::string input = R"(
        [Template] @Element MyComponent {
            p { text { "first" } }
            p { text { "second" } }
            p { text { "third" } }
        }

        body {
            @Element MyComponent {
                delete p[1];
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == "<body><p>first</p><p>third</p></body>");
}

TEST_CASE("Generator handles various element insertions in specialization", "[generator][specialization]") {
    SECTION("Insert after indexed element") {
        std::string input = R"(
            [Template] @Element MyComponent {
                p { text { "first" } }
                p { text { "third" } }
            }

            body {
                @Element MyComponent {
                    insert after p[0] {
                        span { text { "inserted" } }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == "<body><p>first</p><span>inserted</span><p>third</p></body>");
    }

    SECTION("Insert at top") {
        std::string input = R"(
            [Template] @Element MyComponent {
                p { text { "first" } }
            }

            body {
                @Element MyComponent {
                    insert at top {
                        span { text { "inserted" } }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == "<body><span>inserted</span><p>first</p></body>");
    }

    SECTION("Insert at bottom") {
        std::string input = R"(
            [Template] @Element MyComponent {
                p { text { "first" } }
            }

            body {
                @Element MyComponent {
                    insert at bottom {
                        span { text { "inserted" } }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == "<body><p>first</p><span>inserted</span></body>");
    }

    SECTION("Replace indexed element") {
        std::string input = R"(
            [Template] @Element MyComponent {
                p { text { "first" } }
                p { text { "second" } }
            }

            body {
                @Element MyComponent {
                    insert replace p[0] {
                        span { text { "replaced" } }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == "<body><span>replaced</span><p>second</p></body>");
    }
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