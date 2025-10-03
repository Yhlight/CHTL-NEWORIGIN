#include <iostream>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "Util/ASTPrinter/ASTPrinter.h"
#include "Util/AttributeAutomator/AttributeAutomator.h"
#include "Util/CssGenerator/CssGenerator.h"

int main() {
    std::string chtl_code = R"(
        div {
            class: "my-div";
            style {
                width: 100px + 50px * 2;
                &:hover {
                    color: blue;
                }
            }
        }
    )";

    CHTLParser parser(chtl_code);
    parser.parse();

    AttributeAutomator automator;
    automator.process(parser.getRoot());

    CssGenerator cssGenerator;
    cssGenerator.generate(parser.getRoot(), parser.getGlobalStylesheet());

    std::cout << "--- AST ---" << std::endl;
    ASTPrinter printer;
    printer.print(parser.getRoot());
    std::cout << "-----------" << std::endl;

    std::cout << "\n--- Global Stylesheet ---\n";
    std::cout << parser.getGlobalStylesheet().getStylesheet();
    std::cout << "-------------------------\n";

    std::cout << "Parsing complete." << std::endl;

    return 0;
}