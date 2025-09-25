#include "Loader.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLState/ParsingState.h"
#include "../../Scanner/UnifiedScanner.h"
#include "../../Util/FileSystem/File.h"
#include <iostream>

namespace CHTL {

Loader::Loader() {
    // The context is created and owned by the loader.
    // We need to set its initial state.
    context.SetState(std::make_unique<ParsingState>());
}

std::unique_ptr<Node> Loader::LoadFile(const std::string& filepath) {
    ImportFile(filepath);
    // After all imports are resolved and parsed, the final AST root
    // is in the context. We move it out to transfer ownership to the caller.
    return std::move(context.root);
}

void Loader::ImportFile(const std::string& filepath) {
    // Prevent circular imports
    if (loadedFiles.count(filepath)) {
        return;
    }
    loadedFiles.insert(filepath);

    // Read the file
    auto sourceOpt = Util::ReadFile(filepath);
    if (!sourceOpt) {
        std::cerr << "Error: Could not open file to import '" << filepath << "'" << std::endl;
        return;
    }

    // Scan the source to separate CHTL from global CSS
    UnifiedScanner scanner;
    ScanResult scanResult = scanner.Scan(*sourceOpt);

    // Append global CSS to the context
    context.globalCss += scanResult.globalCss;

    // Lex the CHTL-only code
    Lexer lexer(scanResult.chtlCode);
    std::vector<Token> tokens = lexer.Tokenize();

    // Create a parser and parse the tokens into the shared context
    Parser parser(context, *this);
    parser.Parse(std::move(tokens));
}

} // namespace CHTL
