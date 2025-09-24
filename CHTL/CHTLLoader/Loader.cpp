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

    // Save the current namespace to restore it after the import
    std::string originalNamespace = context.currentNamespace;

    // Set the namespace for the new file. Default to filename if not specified.
    // This is a simplified version of the spec's default namespace logic.
    size_t last_slash = filepath.find_last_of("/\\");
    size_t last_dot = filepath.find_last_of('.');
    std::string default_ns = filepath.substr(last_slash == std::string::npos ? 0 : last_slash + 1,
                                            (last_dot == std::string::npos ? std::string::npos : last_dot) - (last_slash == std::string::npos ? 0 : last_slash + 1));
    context.currentNamespace = default_ns;
    if (context.namespaces.find(default_ns) == context.namespaces.end()) {
        context.namespaces[default_ns] = NamespaceData{default_ns};
    }


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

    // Restore the original namespace
    context.currentNamespace = originalNamespace;
}

} // namespace CHTL
