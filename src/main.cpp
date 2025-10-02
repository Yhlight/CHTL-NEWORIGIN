#include <iostream>
#include "CHTL/CHTLParser/CHTLParser.h"

int main() {
    std::string chtl_code = R"(
        div {
            id: "main-container";
            class: "container";

            text {
                "Hello, CHTL!"
            }
        }
    )";

    CHTLParser parser(chtl_code);
    parser.parse();

    std::cout << "Parsing complete." << std::endl;

    return 0;
}