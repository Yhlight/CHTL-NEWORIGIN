#include <iostream>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "Util/ASTPrinter/ASTPrinter.h"
#include "Util/AttributeAutomator/AttributeAutomator.h"

int main() {
    std::string chtl_code = R"(
        // This is the main page structure
        html {
            # This is a generator comment for the head
            head {
                title { text { "My CHTL Page" } }
            }
            body {
                // Main container for the content
                div {
                    id = "main-container";
                    class: "container";

                    h1 { text { "Welcome to CHTL!" } }

                    p {
                        text { "This is a paragraph of text." }
                    }
                }
            }
        }
    )";

    CHTLParser parser(chtl_code);
    parser.parse();

    AttributeAutomator automator;
    automator.process(parser.getRoot());

    std::cout << "--- AST ---" << std::endl;
    ASTPrinter printer;
    printer.print(parser.getRoot());
    std::cout << "-----------" << std::endl;

    std::cout << "Parsing complete." << std::endl;

    return 0;
}