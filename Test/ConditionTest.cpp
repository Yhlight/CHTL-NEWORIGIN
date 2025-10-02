#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>

std::string generate_html(const std::string& input) {
    CHTLContext context;
    CHTLParser parser(input, context);
    auto doc = parser.parse();
    HtmlGenerator generator;
    doc->accept(generator);
    return generator.getResult();
}

TEST_CASE("Conditional rendering: simple if statement, true", "[condition]") {
    const std::string input = R"(
html {
    body {
        style {
            width: 100px;
        }
        if {
            condition: width > 50px,
            display: block;
        }
    }
}
)";
    std::string result = generate_html(input);
    INFO(result);
    REQUIRE(result.find("style=\"width: 100px; display: block; \"") != std::string::npos);
}

TEST_CASE("Conditional rendering: simple if statement, false", "[condition]") {
    const std::string input = R"(
html {
    body {
        style {
            width: 40px;
        }
        if {
            condition: width > 50px,
            display: block;
        }
    }
}
)";
    std::string result = generate_html(input);
    INFO(result);
    REQUIRE(result.find("style=\"width: 40px;\"") != std::string::npos);
    REQUIRE(result.find("display: block;") == std::string::npos);
}

TEST_CASE("Conditional rendering: if-else statement, if true", "[condition]") {
    const std::string input = R"(
html {
    body {
        style {
            width: 60px;
        }
        if {
            condition: width > 50px,
            display: block;
        } else {
            display: none;
        }
    }
}
)";
    std::string result = generate_html(input);
    INFO(result);
    REQUIRE(result.find("style=\"width: 60px; display: block; \"") != std::string::npos);
    REQUIRE(result.find("display: none;") == std::string::npos);
}

TEST_CASE("Conditional rendering: if-else statement, else true", "[condition]") {
    const std::string input = R"(
html {
    body {
        style {
            width: 40px;
        }
        if {
            condition: width > 50px,
            display: block;
        } else {
            display: none;
        }
    }
}
)";
    std::string result = generate_html(input);
    INFO(result);
    REQUIRE(result.find("style=\"width: 40px; display: none; \"") != std::string::npos);
    REQUIRE(result.find("display: block;") == std::string::npos);
}

TEST_CASE("Conditional rendering: if-else if-else chain", "[condition]") {
    const std::string input = R"(
html {
    body {
        style {
            width: 75px;
        }
        if {
            condition: width > 100px,
            color: red;
        } else if {
            condition: width > 50px,
            color: blue;
        } else {
            color: green;
        }
    }
}
)";
    std::string result = generate_html(input);
    INFO(result);
    REQUIRE(result.find("style=\"width: 75px; color: blue; \"") != std::string::npos);
}

TEST_CASE("Conditional rendering: reference another element", "[condition]") {
    const std::string input = R"(
html {
    head {
        style {
            width: 200px;
        }
    }
    body {
        if {
            condition: head.width > 100px,
            background-color: red;
        }
    }
}
)";
    std::string result = generate_html(input);
    INFO(result);
    REQUIRE(result.find("<body style=\"background-color: red; \"") != std::string::npos);
}