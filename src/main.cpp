#include <iostream>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "Util/ASTPrinter/ASTPrinter.h"
#include "Util/AttributeAutomator/AttributeAutomator.h"
#include "Util/CssGenerator/CssGenerator.h"
#include "Util/StyleEvaluator/StyleEvaluator.h"
#include "Util/ImportResolver/ImportResolver.h"
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string chtl_code = buffer.str();

    CHTLParser parser(chtl_code);
    parser.parse();

    ImportResolver importResolver;
    importResolver.resolve(*parser.getRoot());

    AttributeAutomator automator;
    automator.process(parser.getRoot());

    StyleEvaluator styleEvaluator;
    styleEvaluator.evaluate(parser.getRoot());

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