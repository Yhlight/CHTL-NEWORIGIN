#include "CompilerDispatcher.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

#include <iostream> // For error printing

namespace CHTL {

CompilationResult CompilerDispatcher::Dispatch(const ScanResult& scanResult) {
    CompilationResult result;

    // 1. Compile the main CHTL source
    Lexer lexer(scanResult.chtlSource);
    Parser parser(lexer);
    auto program = parser.ParseProgram();

    if (!parser.Errors().empty()) {
        std::cerr << "CHTL Parsing failed with " << parser.Errors().size() << " errors." << std::endl;
        for (const auto& error : parser.Errors()) {
            std::cerr << " - " << error << std::endl;
        }
        // Return an empty result to signal failure
        return {};
    }

    Generator generator;
    result.chtlOutput.content = generator.Generate(program.get());

    // 2. "Compile" the fragments
    for (const auto& [placeholder, fragment] : scanResult.fragments) {
        switch (fragment.type) {
            case FragmentType::CSS:
                // For now, the "CSS Compiler" is a simple pass-through.
                // It just copies the content.
                result.fragmentOutputs[placeholder] = { fragment.content };
                break;
            case FragmentType::JavaScript:
            case FragmentType::CHTL_JS:
                // Handle later
                break;
        }
    }

    return result;
}

} // namespace CHTL
