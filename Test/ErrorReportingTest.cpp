#include "catch.hpp"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../Util/Exceptions/CHTLException.h"
#include "../CHTL/CHTLContext/CHTLContext.h"

TEST_CASE("Parser throws CHTLException with correct line and column number for missing semicolon", "[error]") {
    const std::string input = R"(
html {
    body {
        div {
            style {
                color: red
            }
        }
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);

    try {
        parser.parse();
        FAIL("Parsing should have failed due to a missing semicolon.");
    } catch (const CHTLException& e) {
        // The parser expects a semicolon after "red". The next token is '}' on line 7.
        // Manually counted:
        // line 1: (empty)
        // line 2: html {
        // line 3:     body {
        // line 4:         div {
        // line 5:             style {
        // line 6:                 color: red
        // line 7:             }
        // The '}' is at line 7, column 14 according to the lexer.
        REQUIRE(e.getLine() == 7);
        REQUIRE(e.getColumn() == 14);
    }
}